// JWT
// ===

#include "cachet_common.h"

static int cachet_split_token(const char *token, char **head, char **payload, char **sig) {
  return cachet_jwt_parts(token, head, payload, sig);
}

Term verify_payload_run(Env e, Term *f, IoWork *w) {
  uint64_t token_len = 0;
  uint64_t secret_len = 0;
  char *token = io_cstr(e, f[0], &token_len);
  char *secret = io_cstr(e, f[1], &secret_len);
  char *head_b64 = NULL;
  char *pay_b64 = NULL;
  char *sig_b64 = NULL;
  char *signing = NULL;
  unsigned char *digest = NULL;
  char *expected_sig = NULL;
  unsigned char *payload = NULL;
  size_t signing_len = 0;
  size_t digest_len = 0;
  size_t expected_len = 0;
  size_t payload_len = 0;

  if (cachet_split_token(token, &head_b64, &pay_b64, &sig_b64) != 0) {
    goto fail_format;
  }

  signing_len = strlen(head_b64) + 1 + strlen(pay_b64);
  signing = (char *)malloc(signing_len + 1);
  if (signing == NULL) {
    goto fail;
  }
  snprintf(signing, signing_len + 1, "%s.%s", head_b64, pay_b64);

  if (cachet_hmac_sha256(
        e, signing, signing_len, secret, secret_len, &digest, &digest_len) != 0
    || cachet_b64url_encode(e, digest, digest_len, &expected_sig, &expected_len) != 0) {
    goto fail;
  }

  if (strlen(sig_b64) != expected_len
    || !cachet_ct_eq(sig_b64, expected_sig, expected_len)) {
    free(token);
    free(secret);
    free(head_b64);
    free(pay_b64);
    free(sig_b64);
    free(signing);
    free(digest);
    free(expected_sig);
    return io_fail(e, CACHET_ERR_AUTH, "cachet: invalid signature");
  }

  if (cachet_b64url_decode(e, pay_b64, strlen(pay_b64), &payload, &payload_len) != 0) {
    goto fail;
  }

  free(token);
  free(secret);
  free(head_b64);
  free(pay_b64);
  free(sig_b64);
  free(signing);
  free(digest);
  free(expected_sig);
  Term result = io_str(e, (const char *)payload, payload_len);
  free(payload);
  return io_done(e, result);

fail_format:
  free(token);
  free(secret);
  return io_fail(e, CACHET_ERR_FORMAT, "cachet: invalid token format");

fail:
  free(token);
  free(secret);
  free(head_b64);
  free(pay_b64);
  free(sig_b64);
  free(signing);
  free(digest);
  free(expected_sig);
  free(payload);
  return io_fail(e, ENOMEM, "cachet: verify token failed");
}

static void __attribute__((constructor)) verify_payload_use(void) {
  io_eff(CID_VERIFY_PAYLOAD, verify_payload_run, 0);
}
