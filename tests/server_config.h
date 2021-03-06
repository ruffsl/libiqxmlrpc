#ifndef _iqxmlrpc_test_server_config_h_
#define _iqxmlrpc_test_server_config_h_

#include <stdexcept>

//! Test server configuration structure
struct Test_server_config {
  class Bad_config;

  int port;
  int numthreads;
  bool use_ssl;
  bool omit_string_tags;

  Test_server_config(int argc, char** argv);
};

class Test_server_config::Bad_config: public std::runtime_error {
public:
  Bad_config(const std::string& usage):
    runtime_error(usage) {}
};

#endif
