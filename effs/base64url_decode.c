// Codec
// =====

#include "cachet_common.h"

Term codec_base64url_decode_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *input = io_cstr(e, f[0], &n);
  unsigned char *decoded = NULL;
  size_t decoded_len = 0;
  if (cachet_b64url_decode(e, input, n, &decoded, &decoded_len) != 0) {
    free(input);
    return io_fail(e, CACHET_ERR_FORMAT, "cachet: base64url decode failed");
  }
  free(input);
  Term result = io_str(e, (const char *)decoded, decoded_len);
  free(decoded);
  return io_done(e, result);
}

static void __attribute__((constructor)) codec_base64url_decode_use(void) {
  io_eff(CID_CODEC_BASE64URL_DECODE, codec_base64url_decode_run, 0);
}
