// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_HTTP_HTTP_NETWORK_SESSION_H_
#define NET_HTTP_HTTP_NETWORK_SESSION_H_

#include "base/ref_counted.h"
#include "net/base/host_resolver.h"
#include "net/base/ssl_client_auth_cache.h"
#include "net/base/ssl_config_service.h"
#include "net/http/http_auth_cache.h"
#include "net/proxy/proxy_service.h"
#include "net/socket/tcp_client_socket_pool.h"

namespace net {

class ClientSocketFactory;

// This class holds session objects used by HttpNetworkTransaction objects.
class HttpNetworkSession : public base::RefCounted<HttpNetworkSession> {
 public:
  HttpNetworkSession(HostResolver* host_resolver, ProxyService* proxy_service,
                     ClientSocketFactory* client_socket_factory,
                     SSLConfigService* ssl_config_service);

  HttpAuthCache* auth_cache() { return &auth_cache_; }
  SSLClientAuthCache* ssl_client_auth_cache() {
    return &ssl_client_auth_cache_;
  }

  TCPClientSocketPool* tcp_socket_pool() { return tcp_socket_pool_; }
  HostResolver* host_resolver() { return host_resolver_; }
  ProxyService* proxy_service() { return proxy_service_; }
  SSLConfigService* ssl_config_service() { return ssl_config_service_; }

  static void set_max_sockets_per_group(int socket_count);

 private:
  FRIEND_TEST(HttpNetworkTransactionTest, GroupNameForProxyConnections);

  // Total limit of sockets. Not a constant to allow experiments.
  static int max_sockets_;

  // Default to allow up to 6 connections per host. Experiment and tuning may
  // try other values (greater than 0).  Too large may cause many problems, such
  // as home routers blocking the connections!?!?
  static int max_sockets_per_group_;

  HttpAuthCache auth_cache_;
  SSLClientAuthCache ssl_client_auth_cache_;
  scoped_refptr<TCPClientSocketPool> tcp_socket_pool_;
  scoped_refptr<HostResolver> host_resolver_;
  scoped_refptr<ProxyService> proxy_service_;
  scoped_refptr<SSLConfigService> ssl_config_service_;
};

}  // namespace net

#endif  // NET_HTTP_HTTP_NETWORK_SESSION_H_
