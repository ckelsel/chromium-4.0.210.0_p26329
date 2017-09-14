// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_BASE_HOST_RESOLVER_PROC_H_
#define NET_BASE_HOST_RESOLVER_PROC_H_

#include <string>

#include "base/ref_counted.h"

namespace net {

class AddressList;

// Interface for a getaddrinfo()-like procedure. This is used by unit-tests
// to control the underlying resolutions in HostResolverImpl. HostResolverProcs
// can be chained together; they fallback to the next procedure in the chain
// by calling ResolveUsingPrevious().
//
// Note that implementations of HostResolverProc *MUST BE THREADSAFE*, since
// the HostResolver implementation using them can be multi-threaded.
class HostResolverProc : public base::RefCountedThreadSafe<HostResolverProc> {
 public:
  explicit HostResolverProc(HostResolverProc* previous);
  virtual ~HostResolverProc() {}

  // Resolves |host| to an address list. If successful returns OK and fills
  // |addrlist| with a list of socket addresses. Otherwise returns a
  // network error code.
  virtual int Resolve(const std::string& host, AddressList* addrlist) = 0;

 protected:
  // Asks the fallback procedure (if set) to do the resolve.
  int ResolveUsingPrevious(const std::string& host, AddressList* addrlist);

 private:
  friend class HostResolverImpl;
  friend class MockHostResolverBase;
  friend class ScopedDefaultHostResolverProc;

  // Sets the previous procedure in the chain.
  void set_previous_proc(HostResolverProc* proc) {
    previous_proc_ = proc;
  }

  // Sets the default host resolver procedure that is used by HostResolverImpl.
  // This can be used through ScopedDefaultHostResolverProc to set a catch-all
  // DNS block in unit-tests (individual tests should use MockHostResolver to
  // prevent hitting the network).
  static HostResolverProc* SetDefault(HostResolverProc* proc);
  static HostResolverProc* GetDefault();

 private:
  scoped_refptr<HostResolverProc> previous_proc_;
  static HostResolverProc* default_proc_;

  DISALLOW_COPY_AND_ASSIGN(HostResolverProc);
};

// Resolves |host| to an address list, using the system's default host resolver.
// (i.e. this calls out to getaddrinfo()). If successful returns OK and fills
// |addrlist| with a list of socket addresses. Otherwise returns a
// network error code.
int SystemHostResolverProc(const std::string& host, AddressList* addrlist);

}  // namespace net

#endif  // NET_BASE_HOST_RESOLVER_PROC_H_
