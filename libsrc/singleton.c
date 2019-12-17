//
// Created by linux on 12/3/19.
//

#include "singleton.h"

bool running(const char *program_name)
{
    char filename[100] = {0};
    sprintf(filename, "./%s.pid", program_name);

    int fd = open(filename, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if (fd < 0) {
        printf("open file \"%s\" failed! add sudo\n", filename);
        return true;
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    int ret = fcntl(fd, F_SETLK, &fl);
    if (ret < 0) {
        if(errno == EACCES || errno == EAGAIN) {
            close(fd);
            return true;
        }
    }
    ftruncate(fd, 0);
    char buf[16];
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    return false;
}
