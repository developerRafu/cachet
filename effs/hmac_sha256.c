// Crypto
// ======

#include "cachet_common.h"

Term crypto_hmac_sha256_run(Env e, Term *f, IoWork *w) {
  uint64_t msg_len = 0;
  uint64_t sec_len = 0;
  char *message = io_cstr(e, f[0], &msg_len);
  char *secret = io_cstr(e, f[1], &sec_len);
  unsigned char *digest = NULL;
  size_t digest_len = 0;
  char *encoded = NULL;
  size_t encoded_len = 0;
  if (cachet_hmac_sha256(e, message, msg_len, secret, sec_len, &digest, &digest_len) != 0) {
    free(message);
    free(secret);
    return io_fail(e, ENOMEM, "cachet: hmac failed");
  }
  if (cachet_b64url_encode(e, digest, digest_len, &encoded, &encoded_len) != 0) {
    free(message);
    free(secret);
    free(digest);
    return io_fail(e, ENOMEM, "cachet: hmac encode failed");
  }
  free(message);
  free(secret);
  free(digest);
  Term result = io_str(e, encoded, encoded_len);
  free(encoded);
  return io_done(e, result);
}

static void __attribute__((constructor)) crypto_hmac_sha256_use(void) {
  io_eff(CID_CRYPTO_HMAC_SHA256, crypto_hmac_sha256_run, 0);
}
