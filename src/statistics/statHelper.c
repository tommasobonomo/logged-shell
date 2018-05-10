#include "./statHelper.h"
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * USEFUL:
 * getrusage
 * /usr/bin/time -v "programName"
 * man proc
 * https://www.softprayog.in/tutorials/linux-process-execution-time
 */

void getCurrProcessStats()
{
    printf("### STATISTICS 1 ###\n");

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("user CPU time used: %ld\n", usage.ru_utime.tv_sec + usage.ru_utime.tv_usec);
    printf("system CPU time used: %ld\n", usage.ru_stime.tv_sec + usage.ru_stime.tv_usec);
    printf("maximum resident set size: %ld\n", usage.ru_maxrss);
    printf("integral shared memory size: %ld\n", usage.ru_ixrss);
    printf("integral unshared data size: %ld\n", usage.ru_idrss);
    printf("integral unshared stack size: %ld\n", usage.ru_isrss);
    printf("page reclaims (soft page faults): %ld\n", usage.ru_minflt);
    printf("page faults (hard page faults): %ld\n", usage.ru_majflt);
    printf("swaps: %ld\n", usage.ru_nswap);
    printf("block input operations: %ld\n", usage.ru_inblock);
    printf("block output operations: %ld\n", usage.ru_oublock);
    printf("IPC messages sent: %ld\n", usage.ru_msgsnd);
    printf("IPC messages received: %ld\n", usage.ru_msgrcv);
    printf("signals received: %ld\n", usage.ru_nsignals);
    printf("voluntary context switches: %ld\n", usage.ru_nvcsw);
    printf("involuntary context switches: %ld\n", usage.ru_nivcsw);

    printf("### STATISTICS 2 ###\n");

    char buf[20];
    sprintf(buf, "cat /proc/%d/status", getpid());
    system(buf);
}