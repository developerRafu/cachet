// Claims
// ======

#include "cachet_common.h"

Term pipe_field_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *packed = io_cstr(e, f[0], &n);
  uint32_t index = (uint32_t)f[1];
  char *copy = cachet_strdup(packed, n);
  char *fields[8] = {0};
  int count = 0;
  char *token = strtok(copy, "|");
  while (token != NULL && count < 8) {
    fields[count++] = token;
    token = strtok(NULL, "|");
  }
  free(packed);
  if (index >= (uint32_t)count) {
    free(copy);
    return io_fail(e, CACHET_ERR_FORMAT, "cachet: invalid pipe payload");
  }
  char *field = fields[index];
  size_t field_len = strlen(field);
  Term result = io_str(e, field, field_len);
  free(copy);
  return io_done(e, result);
}

static void __attribute__((constructor)) pipe_field_use(void) {
  io_eff(CID_PIPE_FIELD, pipe_field_run, 0);
}
