// JWT
// ===

#include "cachet_common.h"

Term sign_token_raw_run(Env e, Term *f, IoWork *w) {
  uint64_t payload_len = 0;
  uint64_t secret_len = 0;
  char *payload = io_cstr(e, f[0], &payload_len);
  char *secret = io_cstr(e, f[1], &secret_len);
  const char *header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
  char *head_b64 = NULL;
  char *pay_b64 = NULL;
  unsigned char *digest = NULL;
  char *sig_b64 = NULL;
  char *token = NULL;
  size_t head_b64_len = 0;
  size_t pay_b64_len = 0;
  size_t digest_len = 0;
  size_t sig_b64_len = 0;
  size_t token_len = 0;

  if (cachet_b64url_encode(e, (const unsigned char *)header, strlen(header), &head_b64, &head_b64_len) != 0
    || cachet_b64url_encode(e, (const unsigned char *)payload, payload_len, &pay_b64, &pay_b64_len) != 0) {
    goto fail;
  }

  token_len = head_b64_len + 1 + pay_b64_len;
  token = (char *)malloc(token_len + 1);
  if (token == NULL) {
    goto fail;
  }
  snprintf(token, token_len + 1, "%s.%s", head_b64, pay_b64);

  if (cachet_hmac_sha256(e, token, token_len, secret, secret_len, &digest, &digest_len) != 0
    || cachet_b64url_encode(e, digest, digest_len, &sig_b64, &sig_b64_len) != 0) {
    goto fail;
  }

  free(token);
  token_len = head_b64_len + 1 + pay_b64_len + 1 + sig_b64_len;
  token = (char *)malloc(token_len + 1);
  if (token == NULL) {
    goto fail;
  }
  snprintf(token, token_len + 1, "%s.%s.%s", head_b64, pay_b64, sig_b64);

  free(payload);
  free(secret);
  free(head_b64);
  free(pay_b64);
  free(digest);
  free(sig_b64);
  Term result = io_str(e, token, token_len);
  free(token);
  return io_done(e, result);

fail:
  free(payload);
  free(secret);
  free(head_b64);
  free(pay_b64);
  free(digest);
  free(sig_b64);
  free(token);
  return io_fail(e, ENOMEM, "cachet: sign token failed");
}

static void __attribute__((constructor)) sign_token_raw_use(void) {
  io_eff(CID_SIGN_TOKEN_RAW, sign_token_raw_run, 0);
}
