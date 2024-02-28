#include <stdio.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define SCHED_DONOR 7
#define SCHED_RECVR 8

const struct sched_param param = {.sched_priority = 0};

/* do some busywork, returning the ms taken */
int do_work() {
  clock_t before = clock();

  int n = 10000;
  for (int i = 0; i < n; i++) {
    int acc = 1;
    for (int j = 0; j < i; j++) {
      acc *= j;
      asm("");
    }
  }

  clock_t difference = clock() - before;
  return difference * 1000 / CLOCKS_PER_SEC;
}

/* donor thread */
void parent() {
  if (sched_setscheduler(0, SCHED_DONOR, &param)) {
    printf("error setting scheduler for parent: %d\n", errno);
    exit(1);
  }

  int real_time;
  do {
    real_time = do_work();
    printf("donor: did %dms seconds of work\n", real_time);
    break;
  } while(1);
}

/* receiver thread */
void child() {
  if (sched_setscheduler(0, SCHED_RECVR, &param)) {
    printf("error setting scheduler for child: %d\n", errno);
    exit(1);
  }

  /* can only receive from processes with lower nice values */
  if (setpriority(PRIO_PROCESS, 0, getpriority(PRIO_PROCESS, 0) + 1)) {
    printf("error setting niceness for receiver: %d\n", errno);
    exit(1);
  }

  int real_time;
  do {
    real_time = do_work();
    printf("donor: did %dms seconds of work\n", real_time);
    break;
  } while(1);
}

int main()
{
  pid_t p = fork();
  if(p<0){
    perror("fork fail");
    return 1;
  }
  if (p == 0) {
    parent();
  } else {
    child();
  }

  return 0;
}
