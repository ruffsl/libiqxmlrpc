#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>
#include <libiqxmlrpc/https_client.h>

#include "helper.hpp"

int main()
{
  using namespace iqxmlrpc;

  iqnet::ssl::ctx = iqnet::ssl::Ctx::client_server( "../keys/client.crt", "../keys/client.pem" );

  boost::optional<FingerprintVerifier> server_verifier;
  const std::string server_fingerprint_ = "FF:61:1B:09:6F:C3:CA:0A:58:DA:5E:1A:F6:AC:C0:F6:80:C4:C2:ED:0D:65:02:A9:86:4B:62:F1:E8:2A:09:69";
  server_verifier = FingerprintVerifier(server_fingerprint_);
  iqnet::ssl::ctx->verify_server(&server_verifier.get());

  Client<Https_client_connection> client(iqnet::Inet_addr(3344));

  Param_list pl;
  pl.push_back(Struct());
  pl[0].insert("var1", 1);
  pl[0].insert("var2", "value");

  Response r = client.execute("echo", pl);

  assert(r.value()["var1"].get_int() == 1);
  std::cout << "OK" << std::endl;
}
