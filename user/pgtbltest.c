#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "kernel/types.h"
#include "kernel/riscv.h"
#include "user/user.h"
#include "kernel/vm.h"

#define SZ (8 * SUPERPGSIZE)

void ugetpid_test();

int
main(int argc, char *argv[])
{
  ugetpid_test();
  printf("pgtbltest: all tests succeeded\n");
  exit(0);
}

char *testname = "???";

void
err(char *why)
{
  printf("pgtbltest: %s failed: %s, pid=%d\n", testname, why, getpid());
  exit(1);
}

void
ugetpid_test()
{
  int i;

  printf("ugetpid_test starting\n");
  testname = "ugetpid_test";

  if(getpid() != ugetpid())
    err("mismatched PID #1");

  for (i = 0; i < 64; i++) {
    int ret = fork();
    if (ret != 0) {
      wait(&ret);
      if (ret != 0)
        exit(1);
      continue;
    }
    if (getpid() != ugetpid())
      err("mismatched PID #2");
    exit(0);
  }
  printf("ugetpid_test: OK\n");
}
