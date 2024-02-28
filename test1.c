#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <stdlib.h>

void dofork()
{
	pid_t p = fork();
	if(p<0){
		perror("fork fail");
		exit(1);
	}

	if (p == 0) {
        printf("child niceness = %d\n", nice(0));
		exit(0);
	} else {
        printf("parent niceness = %d\n", nice(0));
    }
}
int main()
{
	const struct sched_param param = {};
	printf("setting scheduler: %d (errno = %d)\n", sched_setscheduler(0, 9, &param), errno);

	dofork();
	dofork();
	dofork();
	dofork();
	dofork();
	dofork();
	return 0;
}
