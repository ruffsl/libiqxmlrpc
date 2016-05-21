#include <iostream>
#include <libiqxmlrpc/libiqxmlrpc.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

std::string
my_cert_finger_sha256(X509_STORE_CTX* ctx) {
  X509* x = X509_STORE_CTX_get_current_cert(ctx);
  const EVP_MD* digest = EVP_get_digestbyname("sha256");
  unsigned int n = 0;
  unsigned char md[EVP_MAX_MD_SIZE];
  X509_digest(x, digest, md, &n);

  std::ostringstream ss;
  for(int i = 0; i < 32; i++)
     ss << boost::format("%02x") % int(md[i]);
//     ss << std::hex << int(md[i]);

  return ss.str();
}

class FingerprintVerifier: public iqnet::ssl::ConnectionVerifier {
public:
  FingerprintVerifier(const std::string& finger):
    finger_(finger)
  {
    boost::erase_all(finger_, ":");
    boost::to_lower(finger_);
  }

private:
  int do_verify(bool, X509_STORE_CTX* ctx) const
  {
    printf("recorded finger: [%s]\n", finger_.c_str());
    printf("observed finger: [%s]\n", my_cert_finger_sha256(ctx).c_str());
    return finger_ == my_cert_finger_sha256(ctx);
  }

  mutable std::string finger_;
};
