// Time
// ====

#include "cachet_common.h"

Term clock_unix_now_run(Env e, Term *f, IoWork *w) {
  return io_done(e, (Term)(uint32_t)time(NULL));
}

static void __attribute__((constructor)) clock_unix_now_use(void) {
  io_eff(CID_CLOCK_UNIX_NOW, clock_unix_now_run, 0);
}
