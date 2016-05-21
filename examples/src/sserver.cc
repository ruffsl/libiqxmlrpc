#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/https_server.h>

#include "helper.hpp"

// Simple method that just returns back first input parameter
class Echo: public iqxmlrpc::Method {
public:
  void execute(const iqxmlrpc::Param_list& params, iqxmlrpc::Value& retval)
  {
    if (params.empty())
      retval = 0;
    else
      retval = params[0];
    std::cout << retval["var1"].get_int() << std::endl;
  }
};

int main()
{
  int port = 3344;

  iqnet::ssl::ctx = iqnet::ssl::Ctx::server_only("../keys/server.crt", "../keys/server.pem" );

  boost::optional<FingerprintVerifier> client_verifier;
  const std::string client_fingerprint_ = "4E:95:60:B2:48:18:9B:38:76:90:EC:A9:29:66:71:87:E7:45:73:74:EE:33:2A:49:52:A4:0E:FE:BB:5C:A8:F9";
  client_verifier = FingerprintVerifier(client_fingerprint_);
  iqnet::ssl::ctx->verify_client(true, &client_verifier.get());

  iqxmlrpc::Serial_executor_factory ef;
  iqxmlrpc::Https_server server(port, &ef);

  iqxmlrpc::register_method<Echo>(server, "echo");

  // optional settings
  server.log_errors( &std::cerr );
  server.enable_introspection();
  server.set_max_request_sz(1024*1024);

  // start server
  server.work();
}
