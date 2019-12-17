
#include "libsrc/global.h"
#include <signal.h>
#include <sys/time.h>

#include "libsrc/singleton.h"
#include "libsrc/daemon.h"
#include "libsrc/usb_chandle_task.h"
#include "libsrc/local_ip_mac.h"

char port[6] = {0};
char name[32] = {0};
char http[32] = {0};

int main(int argc, char *argv[])
{
    if(running("usbhotplug")) {  return 0;  }
    int oc = -1;
    bool daemon_run = false;
    while((oc = getopt(argc, argv, "h:p:n:")) != -1) {
        switch (oc) {
            case 'p':
                strncpy(port, optarg, sizeof(port) - 1);
                break;
            case 'n':
                strncpy(name, optarg, sizeof(name) - 1);
                break;
            case 'h':
                strncpy(http, optarg, sizeof(http) - 1);
                break;
            default:
                printf("Usage: sudo cmd [-h] http: [-p] 1234 [-n] MO \n");
                exit(-1);
        }
    }
    //if(daemon_run) {init_daemon(); }
    if(strlen(port) == 0 || strlen(port) == 0 || strlen(port) == 0) {
        printf("Usage: sudo cmd [-h] http: [-p] 1234 [-n] MO \n");
        exit(-1);
    }
    openlog("usbhotplug", LOG_PID, LOG_USER);
    
    pthread_t usb_thread;
    if(init_usb_thread(&usb_thread) != true) {
        return -1;
    }

    if(pthread_join(usb_thread, NULL) != 0) {
        printf("return fail");
        exit(-1);
    }

    closelog();
    return 0;
}
