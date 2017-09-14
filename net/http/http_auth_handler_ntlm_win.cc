// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// See "SSPI Sample Application" at
// http://msdn.microsoft.com/en-us/library/aa918273.aspx
// and "NTLM Security Support Provider" at
// http://msdn.microsoft.com/en-us/library/aa923611.aspx.

#include "net/http/http_auth_handler_ntlm.h"

#include "base/logging.h"
#include "net/base/net_errors.h"

#pragma comment(lib, "secur32.lib")

namespace {

void ZapString(string16* s) {
  memset(&(*s)[0], 0, s->length() * 2);
}

}  // namespace

namespace net {

HttpAuthHandlerNTLM::HttpAuthHandlerNTLM() :  max_token_len_(0) {
  SecInvalidateHandle(&cred_);
  SecInvalidateHandle(&ctxt_);
}

HttpAuthHandlerNTLM::~HttpAuthHandlerNTLM() {
  ResetSecurityContext();
  if (SecIsValidHandle(&cred_)) {
    FreeCredentialsHandle(&cred_);
    SecInvalidateHandle(&cred_);
  }
  ZapString(&password_);
}

int HttpAuthHandlerNTLM::InitializeBeforeFirstChallenge() {
  DCHECK_EQ("ntlm", scheme_) << "This is not ntlm scheme";

  SEC_WCHAR* package = NTLMSP_NAME;
  PSecPkgInfo pkg_info;
  SECURITY_STATUS status;

  // The following API call is required to get the maximum token length
  // for the scheme.
  // TODO(arindam): Move this (PSecPkgInfo) to a static function.
  status = QuerySecurityPackageInfo(package, &pkg_info);
  if (status != SEC_E_OK) {
    LOG(ERROR) << "Security package " << package << " not found";
    return ERR_UNEXPECTED;
  }
  max_token_len_ = pkg_info->cbMaxToken;
  FreeContextBuffer(pkg_info);

  SEC_WINNT_AUTH_IDENTITY identity;
  identity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
  identity.User =
      reinterpret_cast<USHORT*>(const_cast<wchar_t*>(username_.c_str()));
  identity.UserLength = username_.size();
  identity.Domain =
      reinterpret_cast<USHORT*>(const_cast<wchar_t*>(domain_.c_str()));
  identity.DomainLength = domain_.size();
  identity.Password =
      reinterpret_cast<USHORT*>(const_cast<wchar_t*>(password_.c_str()));
  identity.PasswordLength = password_.size();

  TimeStamp expiry;  // Since the credentials handle doesn't expire, ignore
                     // the expiration time.

  // Pass the username/password to get the credentials handle.
  // Note: If the 5th argument is NULL, it uses the default cached credentials
  // for the logged in user, which can be used for single sign-on.
  status = AcquireCredentialsHandle(NULL,  // pszPrincipal
                                    package,  // pszPackage
                                    SECPKG_CRED_OUTBOUND,  // fCredentialUse
                                    NULL,  // pvLogonID
                                    &identity,  // pAuthData
                                    NULL,  // pGetKeyFn (not used)
                                    NULL,  // pvGetKeyArgument (not used)
                                    &cred_,  // phCredential
                                    &expiry);  // ptsExpiry
  if (status != SEC_E_OK)
    return ERR_UNEXPECTED;

  return OK;
}

int HttpAuthHandlerNTLM::GetNextToken(const void* in_token,
                                      uint32 in_token_len,
                                      void** out_token,
                                      uint32* out_token_len) {
  SECURITY_STATUS status;
  TimeStamp expiry;

  DWORD ctxt_attr;
  CtxtHandle* ctxt_ptr;
  SecBufferDesc in_buffer_desc, out_buffer_desc;
  SecBufferDesc* in_buffer_desc_ptr;
  SecBuffer in_buffer, out_buffer;

  if (in_token) {
    // Prepare input buffer.
    in_buffer_desc.ulVersion = SECBUFFER_VERSION;
    in_buffer_desc.cBuffers = 1;
    in_buffer_desc.pBuffers = &in_buffer;
    in_buffer.BufferType = SECBUFFER_TOKEN;
    in_buffer.cbBuffer = in_token_len;
    in_buffer.pvBuffer = const_cast<void*>(in_token);
    ctxt_ptr = &ctxt_;
    in_buffer_desc_ptr = &in_buffer_desc;
  } else {
    // If there is no input token, then we are starting a new authentication
    // sequence.  If we have already initialized our security context, then
    // we're incorrectly reusing the auth handler for a new sequence.
    if (SecIsValidHandle(&ctxt_)) {
      LOG(ERROR) << "Cannot restart authentication sequence";
      return ERR_UNEXPECTED;
    }
    ctxt_ptr = NULL;
    in_buffer_desc_ptr = NULL;
  }

  // Prepare output buffer.
  out_buffer_desc.ulVersion = SECBUFFER_VERSION;
  out_buffer_desc.cBuffers = 1;
  out_buffer_desc.pBuffers = &out_buffer;
  out_buffer.BufferType = SECBUFFER_TOKEN;
  out_buffer.cbBuffer = max_token_len_;
  out_buffer.pvBuffer = malloc(out_buffer.cbBuffer);
  if (!out_buffer.pvBuffer)
    return ERR_OUT_OF_MEMORY;

  // Name of the destination server.  NULL for NTLM.
  SEC_WCHAR* target = NULL;

  // This returns a token that is passed to the remote server.
  status = InitializeSecurityContext(&cred_,  // phCredential
                                     ctxt_ptr,  // phContext
                                     target,  // pszTargetName
                                     0,  // fContextReq
                                     0,  // Reserved1 (must be 0)
                                     SECURITY_NATIVE_DREP,  // TargetDataRep
                                     in_buffer_desc_ptr,  // pInput
                                     0,  // Reserved2 (must be 0)
                                     &ctxt_,  // phNewContext
                                     &out_buffer_desc,  // pOutput
                                     &ctxt_attr,  // pfContextAttr
                                     &expiry);  // ptsExpiry
  // On success, the function returns SEC_I_CONTINUE_NEEDED on the first call
  // and SEC_E_OK on the second call.  On failure, the function returns an
  // error code.
  if (status != SEC_I_CONTINUE_NEEDED && status != SEC_E_OK) {
    LOG(ERROR) << "InitializeSecurityContext failed: " << status;
    ResetSecurityContext();
    free(out_buffer.pvBuffer);
    return ERR_UNEXPECTED;  // TODO(wtc): map error code.
  }
  if (!out_buffer.cbBuffer) {
    free(out_buffer.pvBuffer);
    out_buffer.pvBuffer = NULL;
  }
  *out_token = out_buffer.pvBuffer;
  *out_token_len = out_buffer.cbBuffer;
  return OK;
}

// Require identity on first pass instead of second.
bool HttpAuthHandlerNTLM::NeedsIdentity() {
  return auth_data_.empty();
}

bool HttpAuthHandlerNTLM::IsFinalRound() {
  return !auth_data_.empty();
}

void HttpAuthHandlerNTLM::ResetSecurityContext() {
  if (SecIsValidHandle(&ctxt_)) {
    DeleteSecurityContext(&ctxt_);
    SecInvalidateHandle(&ctxt_);
  }
}

}  // namespace net

