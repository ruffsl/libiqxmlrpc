#include "helper.hpp"
#include <libiqxmlrpc/https_server.h>

// Simple method that just returns back first input parameter
class Echo: public iqxmlrpc::Method {
public:
  void execute(const iqxmlrpc::Param_list& params, iqxmlrpc::Value& retval)
  {
    if (params.empty())
      retval = 0;
    else
      retval = params[0];
    std::cout << retval["var1"].get_string() << std::endl;
  }
};

int main(int argc, const char* argv[])
{
  int mode;
  bool finger = false;
  bool caverify = false;
  if ( argc > 1 ) {
      mode = std::atoi( argv[1] );
  }

  switch (mode) {
  case FINGER:
      finger = true;
      break;
  case CAVERIFY:
      caverify = true;
      break;
  case FINGER_CAVERIFY:
      finger = true;
      caverify = true;
      break;
  default:
      break;
  }

  int port = 3344;
  std::string server_cert_file = "../keys/good/server/server.cert";
  std::string server_key_file = "../keys/good/server/server.pem";
  std::string client_cert_file = "../keys/good/client/client.cert";
  std::string ca_file;
  std::string ca_path = "../keys/good/public";
  iqnet::ssl::ctx = iqnet::ssl::Ctx::server_only(server_cert_file, server_key_file);

  boost::optional<FingerprintVerifier> client_verifier;
  client_verifier = FingerprintVerifier(client_cert_file);
  if(finger)
      iqnet::ssl::ctx->verify_client(true, &client_verifier.get());

  CAVerifier prepare_verifier(ca_file, ca_path);
  SSL* ssl_context = SSL_new(prepare_verifier.prepare(iqnet::ssl::ctx->context()));
  if(caverify)
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
