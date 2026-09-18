// JWT
// ===

#include "cachet_common.h"

Term jwt_token_parts_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *token = io_cstr(e, f[0], &n);
  char *copy = cachet_strdup(token, n);
  char *fields[3] = {0};
  int count = 0;
  char *part = strtok(copy, ".");
  while (part != NULL && count < 3) {
    fields[count++] = part;
    part = strtok(NULL, ".");
  }
  free(token);
  if (count != 3) {
    free(copy);
    return io_fail(e, CACHET_ERR_FORMAT, "cachet: invalid token format");
  }
  char packed[4096];
  snprintf(
    packed,
    sizeof(packed),
    "%s|%s|%s",
    fields[0],
    fields[1],
    fields[2]);
  free(copy);
  return io_done(e, io_str(e, packed, strlen(packed)));
}

static void __attribute__((constructor)) jwt_token_parts_use(void) {
  io_eff(CID_JWT_TOKEN_PARTS, jwt_token_parts_run, 0);
}
