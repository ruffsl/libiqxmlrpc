#include "helper.hpp"
#include <libiqxmlrpc/https_client.h>

int main()
{
  using namespace iqxmlrpc;
  std::string client_cert_file = "../keys/good/client/client.cert";
  std::string client_key_file = "../keys/good/client/client.pem";
  std::string server_cert_file = "../keys/good/server/server.cert";
  std::string ca_file;
  std::string ca_path = "../keys/good/public";
  iqnet::ssl::ctx = iqnet::ssl::Ctx::client_server(client_cert_file, client_key_file);

  boost::optional<FingerprintVerifier> server_verifier;
  server_verifier = FingerprintVerifier(server_cert_file);
  iqnet::ssl::ctx->verify_server(&server_verifier.get());

  CAVerifier prepare_verifier(ca_file, ca_path);
  SSL* ssl_context = SSL_new(prepare_verifier.prepare(iqnet::ssl::ctx->context()));
  iqnet::ssl::ctx->prepare_verify(ssl_context, true);

  Client<Https_client_connection> client(iqnet::Inet_addr(3344));

  Param_list pl;
  pl.push_back(Struct());
  pl[0].insert("var1", "💻 The Answer to the Ultimate Question of Life, The Universe, and Everything?");
  pl[0].insert("var2", "42 📡");

  Response r = client.execute("echo", pl);

  assert(r.value()["var2"].get_string() == "42 📡");
  std::cout << r.value()["var2"].get_string() << std::endl;
}
