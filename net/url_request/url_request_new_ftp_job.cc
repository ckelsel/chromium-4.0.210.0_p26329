// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "net/url_request/url_request_new_ftp_job.h"

#include "base/compiler_specific.h"
#include "base/file_version_info.h"
#include "base/message_loop.h"
#include "base/sys_string_conversions.h"
#include "net/base/auth.h"
#include "net/base/escape.h"
#include "net/base/net_errors.h"
#include "net/base/net_util.h"
#include "net/ftp/ftp_response_info.h"
#include "net/ftp/ftp_server_type_histograms.h"
#include "net/ftp/ftp_transaction_factory.h"
#include "net/third_party/parseftp/ParseFTPList.h"
#include "net/url_request/url_request.h"
#include "net/url_request/url_request_context.h"
#include "net/url_request/url_request_error_job.h"
#include "unicode/ucsdet.h"

namespace {

// A very simple-minded character encoding detection.
// TODO(jungshik): We can apply more heuristics here (e.g. using various hints
// like TLD, the UI language/default encoding of a client, etc). In that case,
// this should be pulled out of here and moved somewhere in base because there
// can be other use cases.
std::string DetectEncoding(const char* input, size_t len) {
  if (IsStringASCII(std::string(input, len)))
    return std::string();
  UErrorCode status = U_ZERO_ERROR;
  UCharsetDetector* detector = ucsdet_open(&status);
  ucsdet_setText(detector, input, static_cast<int32_t>(len), &status);
  const UCharsetMatch* match = ucsdet_detect(detector, &status);
  const char* encoding = ucsdet_getName(match, &status);
  // Should we check the quality of the match? A rather arbitrary number is
  // assigned by ICU and it's hard to come up with a lower limit.
  if (U_FAILURE(status))
    return std::string();
  return encoding;
}

string16 RawByteSequenceToFilename(const char* raw_filename,
                                   const std::string& encoding) {
  if (encoding.empty())
    return ASCIIToUTF16(raw_filename);

  // Try the detected encoding before falling back to the native codepage.
  // Using the native codepage does not make much sense, but we don't have
  // much else to resort to.
  string16 filename;
  if (!CodepageToUTF16(raw_filename, encoding.c_str(),
                       OnStringUtilConversionError::SUBSTITUTE, &filename))
    filename = WideToUTF16Hack(base::SysNativeMBToWide(raw_filename));
  return filename;
}

}  // namespace

URLRequestNewFtpJob::URLRequestNewFtpJob(URLRequest* request)
    : URLRequestJob(request),
      response_info_(NULL),
      dir_listing_buf_size_(0),
      ALLOW_THIS_IN_INITIALIZER_LIST(
          start_callback_(this, &URLRequestNewFtpJob::OnStartCompleted)),
      ALLOW_THIS_IN_INITIALIZER_LIST(
          read_callback_(this, &URLRequestNewFtpJob::OnReadCompleted)),
      read_in_progress_(false),
      context_(request->context()) {
}

URLRequestNewFtpJob::~URLRequestNewFtpJob() {
}

// static
URLRequestJob* URLRequestNewFtpJob::Factory(URLRequest* request,
                                            const std::string& scheme) {
  DCHECK_EQ(scheme, "ftp");

  int port = request->url().IntPort();
  if (request->url().has_port() &&
    !net::IsPortAllowedByFtp(port) && !net::IsPortAllowedByOverride(port))
    return new URLRequestErrorJob(request, net::ERR_UNSAFE_PORT);

  DCHECK(request->context());
  DCHECK(request->context()->ftp_transaction_factory());
  return new URLRequestNewFtpJob(request);
}

void URLRequestNewFtpJob::Start() {
  DCHECK(!transaction_.get());
  request_info_.url = request_->url();
  StartTransaction();
}

void URLRequestNewFtpJob::Kill() {
  if (!transaction_.get())
    return;
  DestroyTransaction();
  URLRequestJob::Kill();
}

net::LoadState URLRequestNewFtpJob::GetLoadState() const {
  return transaction_.get() ?
      transaction_->GetLoadState() : net::LOAD_STATE_IDLE;
}

bool URLRequestNewFtpJob::NeedsAuth() {
  // Note that we only have to worry about cases where an actual FTP server
  // requires auth (and not a proxy), because connecting to FTP via proxy
  // effectively means the browser communicates via HTTP, and uses HTTP's
  // Proxy-Authenticate protocol when proxy servers require auth.
  return server_auth_ && server_auth_->state == net::AUTH_STATE_NEED_AUTH;
}

void URLRequestNewFtpJob::GetAuthChallengeInfo(
    scoped_refptr<net::AuthChallengeInfo>* result) {
  DCHECK((server_auth_ != NULL) &&
         (server_auth_->state == net::AUTH_STATE_NEED_AUTH));
  scoped_refptr<net::AuthChallengeInfo> auth_info = new net::AuthChallengeInfo;
  auth_info->is_proxy = false;
  auth_info->host_and_port = ASCIIToWide(
      net::GetHostAndPort(request_->url()));
  auth_info->scheme = L"";
  auth_info->realm = L"";
  result->swap(auth_info);
}

void URLRequestNewFtpJob::SetAuth(const std::wstring& username,
                                  const std::wstring& password) {
  DCHECK(NeedsAuth());
  server_auth_->state = net::AUTH_STATE_HAVE_AUTH;
  server_auth_->username = username;
  server_auth_->password = password;

  RestartTransactionWithAuth();
}

void URLRequestNewFtpJob::CancelAuth() {
  DCHECK(NeedsAuth());
  server_auth_->state = net::AUTH_STATE_CANCELED;

  // Once the auth is cancelled, we proceed with the request as though
  // there were no auth.  Schedule this for later so that we don't cause
  // any recursing into the caller as a result of this call.
  MessageLoop::current()->PostTask(FROM_HERE, NewRunnableMethod(
      this, &URLRequestNewFtpJob::OnStartCompleted, net::OK));
}

bool URLRequestNewFtpJob::ReadRawData(net::IOBuffer* buf,
                                      int buf_size,
                                      int *bytes_read) {
  DCHECK_NE(buf_size, 0);
  DCHECK(bytes_read);
  DCHECK(!read_in_progress_);
  if (response_info_ == NULL) {
     response_info_ = transaction_->GetResponseInfo();
    if (response_info_->is_directory_listing) {
      std::string escaped_path =
          UnescapeURLComponent(request_->url().path(),
          UnescapeRule::SPACES | UnescapeRule::URL_SPECIAL_CHARS);
      string16 path_utf16;
      // Per RFC 2640, FTP servers should use UTF-8 or its proper subset ASCII,
      // but many old FTP servers use legacy encodings. Try UTF-8 first and
      // detect the encoding.
      if (IsStringUTF8(escaped_path)) {
        path_utf16 = UTF8ToUTF16(escaped_path);
      } else {
        std::string encoding = DetectEncoding(escaped_path.c_str(),
                                              escaped_path.size());
        // Try the detected encoding. If it fails, resort to the
        // OS native encoding.
        if (encoding.empty() ||
            !CodepageToUTF16(escaped_path, encoding.c_str(),
                             OnStringUtilConversionError::SUBSTITUTE,
                             &path_utf16))
          path_utf16 = WideToUTF16Hack(base::SysNativeMBToWide(escaped_path));
      }

      directory_html_ = net::GetDirectoryListingHeader(path_utf16);
      // If this isn't top level directory (i.e. the path isn't "/",)
      // add a link to the parent directory.
      if (request_->url().path().length() > 1)
        directory_html_.append(
            net::GetDirectoryListingEntry(ASCIIToUTF16(".."),
                                          std::string(),
                                          false, 0,
                                          base::Time()));
    }
  }
  if (!directory_html_.empty()) {
    size_t bytes_to_copy = std::min(static_cast<size_t>(buf_size),
                                    directory_html_.size());
    memcpy(buf->data(), directory_html_.c_str(), bytes_to_copy);
    *bytes_read = bytes_to_copy;
    directory_html_.erase(0, bytes_to_copy);
    return true;
  }

  int rv = transaction_->Read(buf, buf_size, &read_callback_);
  if (rv >= 0) {
    if (response_info_->is_directory_listing) {
      *bytes_read = ProcessFtpDir(buf, buf_size, rv);
    } else {
      *bytes_read = rv;
    }
    return true;
  }

  if (response_info_->is_directory_listing) {
    dir_listing_buf_ = buf;
    dir_listing_buf_size_ = buf_size;
  }

  if (rv == net::ERR_IO_PENDING) {
    read_in_progress_ = true;
    SetStatus(URLRequestStatus(URLRequestStatus::IO_PENDING, 0));
  } else {
    NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, rv));
  }
  return false;
}

int URLRequestNewFtpJob::ProcessFtpDir(net::IOBuffer *buf,
                                       int buf_size,
                                       int bytes_read) {
  std::string file_entry;
  std::string line;

  // If all we've seen so far is ASCII, encoding_ is empty. Try to detect the
  // encoding. We don't do the separate UTF-8 check here because the encoding
  // detection with a longer chunk (as opposed to the relatively short path
  // component of the url) is unlikely to mistake UTF-8 for a legacy encoding.
  // If it turns out to be wrong, a separate UTF-8 check has to be added.
  //
  // TODO(jungshik): UTF-8 has to be 'enforced' without any heuristics when
  // we're talking to an FTP server compliant to RFC 2640 (that is, its response
  // to FEAT command includes 'UTF8').
  // See http://wiki.filezilla-project.org/Character_Set
  if (encoding_.empty())
    encoding_ = DetectEncoding(buf->data(), bytes_read);

  int64 file_size;
  std::istringstream iss(std::string(buf->data(), bytes_read));
  struct net::list_state state;
  memset(&state, 0, sizeof(state));
  while (getline(iss, line)) {
    struct net::list_result result;
    std::replace(line.begin(), line.end(), '\r', '\0');
    int line_type = net::ParseFTPList(line.c_str(), &state, &result);

    // The original code assumed months are in range 0-11 (PRExplodedTime),
    // but our Time class expects a 1-12 range. Adjust it here, because
    // the third-party parsing code uses bit-shifting on the month,
    // and it'd be too easy to break that logic.
    result.fe_time.month++;
    DCHECK_LE(1, result.fe_time.month);
    DCHECK_GE(12, result.fe_time.month);

    switch (line_type) {
      case 'd':  // Directory entry.
        file_entry.append(net::GetDirectoryListingEntry(
            RawByteSequenceToFilename(result.fe_fname, encoding_),
            result.fe_fname, true, 0,
            base::Time::FromLocalExploded(result.fe_time)));
        break;
      case 'f':  // File entry.
        if (StringToInt64(result.fe_size, &file_size))
          file_entry.append(net::GetDirectoryListingEntry(
              RawByteSequenceToFilename(result.fe_fname, encoding_),
              result.fe_fname, false, file_size,
              base::Time::FromLocalExploded(result.fe_time)));
        break;
      case 'l': {  // Symlink entry.
          std::string filename(result.fe_fname, result.fe_fnlen);

          // Parsers for styles 'U' and 'W' handle " -> " themselves.
          if (state.lstyle != 'U' && state.lstyle != 'W') {
            std::string::size_type offset = filename.find(" -> ");
            if (offset != std::string::npos)
              filename = filename.substr(0, offset);
          }

          if (StringToInt64(result.fe_size, &file_size)) {
            file_entry.append(net::GetDirectoryListingEntry(
                RawByteSequenceToFilename(filename.c_str(), encoding_),
                filename, false, file_size,
                base::Time::FromLocalExploded(result.fe_time)));
          }
        }
        break;
      case '?':  // Junk entry.
      case '"':  // Comment entry.
        break;
      default:
        NOTREACHED();
        break;
    }
  }
  LogFtpServerType(state);
  directory_html_.append(file_entry);
  size_t bytes_to_copy = std::min(static_cast<size_t>(buf_size),
                                  directory_html_.length());
  if (bytes_to_copy) {
    memcpy(buf->data(), directory_html_.c_str(), bytes_to_copy);
    directory_html_.erase(0, bytes_to_copy);
  }
  return bytes_to_copy;
}

void URLRequestNewFtpJob::LogFtpServerType(
    const struct net::list_state& list_state) {
  // We can't recognize server type based on empty directory listings. Don't log
  // that as unknown, it's misleading.
  if (!list_state.parsed_one)
    return;

  switch (list_state.lstyle) {
    case 'E':
      net::UpdateFtpServerTypeHistograms(net::SERVER_EPLF);
      break;
    case 'V':
      net::UpdateFtpServerTypeHistograms(net::SERVER_VMS);
      break;
    case 'C':
      net::UpdateFtpServerTypeHistograms(net::SERVER_CMS);
      break;
    case 'W':
      net::UpdateFtpServerTypeHistograms(net::SERVER_DOS);
      break;
    case 'O':
      net::UpdateFtpServerTypeHistograms(net::SERVER_OS2);
      break;
    case 'U':
      net::UpdateFtpServerTypeHistograms(net::SERVER_LSL);
      break;
    case 'w':
      net::UpdateFtpServerTypeHistograms(net::SERVER_W16);
      break;
    case 'D':
      net::UpdateFtpServerTypeHistograms(net::SERVER_DLS);
      break;
    default:
      net::UpdateFtpServerTypeHistograms(net::SERVER_UNKNOWN);
      break;
  }
}

void URLRequestNewFtpJob::OnStartCompleted(int result) {
  // If the request was destroyed, then there is no more work to do.
  if (!request_ || !request_->delegate())
    return;
  // If the transaction was destroyed, then the job was cancelled, and
  // we can just ignore this notification.
  if (!transaction_.get())
    return;
  // Clear the IO_PENDING status
  SetStatus(URLRequestStatus());
  if (result == net::OK) {
    NotifyHeadersComplete();
  } else if (transaction_->GetResponseInfo()->needs_auth) {
    server_auth_ = new net::AuthData();
    server_auth_->state = net::AUTH_STATE_NEED_AUTH;
    NotifyHeadersComplete();
  } else {
    NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, result));
  }
}

void URLRequestNewFtpJob::OnReadCompleted(int result) {
  read_in_progress_ = false;
  if (result == 0) {
    NotifyDone(URLRequestStatus());
  } else if (result < 0) {
    NotifyDone(URLRequestStatus(URLRequestStatus::FAILED, result));
  } else {
    // TODO(ibrar): find the best place to delete dir_listing_buf_
    // Filter for Directory listing.
    if (response_info_->is_directory_listing)
      result = ProcessFtpDir(dir_listing_buf_, dir_listing_buf_size_, result);

    // Clear the IO_PENDING status
    SetStatus(URLRequestStatus());
  }
  NotifyReadComplete(result);
}

void URLRequestNewFtpJob::RestartTransactionWithAuth() {
  DCHECK(server_auth_ && server_auth_->state == net::AUTH_STATE_HAVE_AUTH);

  response_info_ = NULL;

  // No matter what, we want to report our status as IO pending since we will
  // be notifying our consumer asynchronously via OnStartCompleted.
  SetStatus(URLRequestStatus(URLRequestStatus::IO_PENDING, 0));

  int rv = transaction_->RestartWithAuth(server_auth_->username,
                                         server_auth_->password,
                                         &start_callback_);
  if (rv == net::ERR_IO_PENDING)
    return;

  MessageLoop::current()->PostTask(FROM_HERE, NewRunnableMethod(
      this, &URLRequestNewFtpJob::OnStartCompleted, rv));
}

void URLRequestNewFtpJob::StartTransaction() {
  // Create a transaction.
  DCHECK(!transaction_.get());
  DCHECK(request_->context());
  DCHECK(request_->context()->ftp_transaction_factory());

  transaction_.reset(
  request_->context()->ftp_transaction_factory()->CreateTransaction());

  // No matter what, we want to report our status as IO pending since we will
  // be notifying our consumer asynchronously via OnStartCompleted.
  SetStatus(URLRequestStatus(URLRequestStatus::IO_PENDING, 0));
  int rv;
  if (transaction_.get()) {
    rv = transaction_->Start(
        &request_info_, &start_callback_, request_->load_log());
    if (rv == net::ERR_IO_PENDING)
      return;
  } else {
    rv = net::ERR_FAILED;
  }
  // The transaction started synchronously, but we need to notify the
  // URLRequest delegate via the message loop.
  MessageLoop::current()->PostTask(FROM_HERE, NewRunnableMethod(
      this, &URLRequestNewFtpJob::OnStartCompleted, rv));
}

void URLRequestNewFtpJob::DestroyTransaction() {
  DCHECK(transaction_.get());

  transaction_.reset();
  response_info_ = NULL;
}
