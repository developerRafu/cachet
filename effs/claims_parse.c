// Claims
// ======

#include "cachet_common.h"

Term parse_raw_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *json = io_cstr(e, f[0], &n);
  char sub[512];
  char iss[512];
  char aud[512];
  uint32_t exp = 0;
  uint32_t iat = 0;
  char packed[2048];
  if (cachet_json_string(json, "sub", sub, sizeof(sub)) != 0
    || cachet_json_string(json, "iss", iss, sizeof(iss)) != 0
    || cachet_json_string(json, "aud", aud, sizeof(aud)) != 0
    || cachet_json_u32(json, "exp", &exp) != 0
    || cachet_json_u32(json, "iat", &iat) != 0) {
    free(json);
    return io_fail(e, CACHET_ERR_FORMAT, "cachet: claims parse failed");
  }
  free(json);
  snprintf(packed, sizeof(packed), "%s|%s|%s|%u|%u", sub, iss, aud, exp, iat);
  return io_done(e, io_str(e, packed, strlen(packed)));
}

static void __attribute__((constructor)) parse_raw_use(void) {
  io_eff(CID_PARSE_RAW, parse_raw_run, 0);
}
