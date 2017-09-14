// Copyright (c) 2009 The Chromium Authors. All rights reserved.  Use of this
// source code is governed by a BSD-style license that can be found in the
// LICENSE file.

#ifndef NET_FTP_FTP_CTRL_RESPONSE_BUFFER_H_
#define NET_FTP_FTP_CTRL_RESPONSE_BUFFER_H_

#include <queue>
#include <string>
#include <vector>

#include "base/basictypes.h"

namespace net {

struct FtpCtrlResponse {
  static const int kInvalidStatusCode;

  FtpCtrlResponse() : status_code(kInvalidStatusCode) {}

  int status_code;                 // Three-digit status code.
  std::vector<std::string> lines;  // Response lines, without CRLFs.
};

class FtpCtrlResponseBuffer {
 public:
  FtpCtrlResponseBuffer() {}

  // Called when data is received from the control socket. Returns error code.
  int ConsumeData(const char* data, int data_length);

  bool ResponseAvailable() const {
    return !responses_.empty();
  }

  // Returns the next response. It is an error to call this function
  // unless ResponseAvailable returns true.
  FtpCtrlResponse PopResponse() {
    FtpCtrlResponse result = responses_.front();
    responses_.pop();
    return result;
  }

 private:
  struct ParsedLine {
    ParsedLine()
        : has_status_code(false),
          is_multiline(false),
          is_complete(false),
          status_code(FtpCtrlResponse::kInvalidStatusCode) {
    }

    // Indicates that this line begins with a valid 3-digit status code.
    bool has_status_code;

    // Indicates that this line has the dash (-) after the code, which
    // means a multiline response.
    bool is_multiline;

    // Indicates that this line could be parsed as a complete and valid
    // response line, without taking into account preceding lines (which
    // may change its meaning into a continuation of the previous line).
    bool is_complete;

    // Part of response parsed as status code.
    int status_code;

    // Part of response parsed as status text.
    std::string status_text;

    // Text before parsing, without terminating CRLF.
    std::string raw_text;
  };

  static ParsedLine ParseLine(const std::string& line);

  void ExtractFullLinesFromBuffer();

  // We keep not-yet-parsed data in a string buffer.
  std::string buffer_;

  std::queue<ParsedLine> lines_;

  // When parsing a multiline response, we don't know beforehand if a line
  // will have a continuation. So always store last line of multiline response
  // so we can append the continuation to it.
  std::string line_buf_;

  // Keep the response data while we add all lines to it.
  FtpCtrlResponse response_buf_;

  // As we read full responses (possibly multiline), we add them to the queue.
  std::queue<FtpCtrlResponse> responses_;

  DISALLOW_COPY_AND_ASSIGN(FtpCtrlResponseBuffer);
};

}  // namespace net

#endif  // NET_FTP_FTP_CTRL_RESPONSE_BUFFER_H_
