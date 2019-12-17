//
// Created by linux on 12/2/19.
//

#include "daemon.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>

void init_daemon(void)
{
    pid_t pid = fork();

    if(pid > 0) {
        exit(0);
    } else if(pid < 0) {
        exit(-1);
    }

    setsid();
    //ignore terminal relate signal
    struct sigaction sa = {
        .sa_handler = SIG_IGN,
        .sa_flags = 0
    };
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGHUP, &sa, NULL) < 0) {
        return;
    }

    if(pid = fork()) {
        exit(0);
    } else if(pid < 0) {
        exit(1);
    }

    struct rlimit rl = {0};
    getrlimit(RLIMIT_NOFILE, &rl);

//    for(int i = 0; i < getdtablesize(); i++) {
//        close(i);
//    }
    for(int i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    chdir("/");
    umask(0);
    return;
}