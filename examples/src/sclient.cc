#include "helper.hpp"
#include <libiqxmlrpc/https_client.h>

int main()
{
  using namespace iqxmlrpc;
  std::string cert_path = "../keys/good/client/client.cert";
  std::string key_path = "../keys/good/client/client.pem";
  std::string ca_file;
  std::string ca_path = "../keys/good/public";
  iqnet::ssl::ctx = iqnet::ssl::Ctx::client_server(cert_path, key_path);

  CAVerifier prepare_verifier(ca_file, ca_path);
  SSL* ssl_context = SSL_new(prepare_verifier.prepare(iqnet::ssl::ctx->context()));
  iqnet::ssl::ctx->prepare_verify(ssl_context, true);

  Client<Https_client_connection> client(iqnet::Inet_addr(3344));

  Param_list pl;
  pl.push_back(Struct());
  pl[0].insert("var1", 1);
  pl[0].insert("var2", "value");

  Response r = client.execute("echo", pl);

  assert(r.value()["var1"].get_int() == 1);
  std::cout << "OK" << std::endl;
}
