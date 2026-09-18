// Codec
// =====

#include "cachet_common.h"

Term codec_base64url_encode_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *input = io_cstr(e, f[0], &n);
  char *encoded = NULL;
  size_t encoded_len = 0;
  if (cachet_b64url_encode(e, (const unsigned char *)input, n, &encoded, &encoded_len) != 0) {
    free(input);
    return io_fail(e, ENOMEM, "cachet: base64url encode failed");
  }
  free(input);
  Term result = io_str(e, encoded, encoded_len);
  free(encoded);
  return io_done(e, result);
}

static void __attribute__((constructor)) codec_base64url_encode_use(void) {
  io_eff(CID_CODEC_BASE64URL_ENCODE, codec_base64url_encode_run, 0);
}
