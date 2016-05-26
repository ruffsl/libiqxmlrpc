#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/https_client.h>

#include "helper.hpp"

int main()
{
  using namespace iqxmlrpc;
  iqnet::ssl::ctx = iqnet::ssl::Ctx::client_server("../keys/good/client/client.cert", "../keys/good/client/client.pem");

  std::string ca_file;
  std::string ca_path = "../keys/good/public";
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
