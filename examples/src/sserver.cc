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

  iqnet::ssl::ctx = iqnet::ssl::Ctx::server_only("../keys/good/server/server.cert", "../keys/good/server/server.pem" );

  const std::string CAfile;
  const std::string CApath = "../keys/good/public";
  CAVerifier prepare_verifier(CAfile, CApath);
  SSL* ssl_context = SSL_new(prepare_verifier.prepare(iqnet::ssl::ctx->context()));
  iqnet::ssl::ctx->prepare_verify(ssl_context, true);

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
