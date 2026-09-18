// Codec
// =====

#include "cachet_common.h"

Term codec_u32_of_string_run(Env e, Term *f, IoWork *w) {
  uint64_t n = 0;
  char *text = io_cstr(e, f[0], &n);
  unsigned int value = 0;
  if (sscanf(text, "%u", &value) != 1) {
    free(text);
    return io_fail(e, CACHET_ERR_FORMAT, "cachet: invalid u32");
  }
  free(text);
  return io_done(e, (Term)value);
}

static void __attribute__((constructor)) codec_u32_of_string_use(void) {
  io_eff(CID_CODEC_U32_OF_STRING, codec_u32_of_string_run, 0);
}
