#include <sys/cdefs.h>

#include <sys/stat.h>
#include <sys/time.h>

#include <libgen.h>
#include <err.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>


#include <unistd.h>

#ifndef UTIME_NOW
#define UTIME_NOW   -1
#define UTIME_OMIT  -2
#endif

#define ATOI2(ar)   ((ar)[0] - '0') * 10 + ((ar)[1] - '0'); (ar) += 2;

int main(int argc, char **argv)
{
    struct stat sb;
    char *myname;
    struct timespec ts[2];
    int atflag, c;
    int fd;

    atflag = 0;

    ts[0].tv_sec = ts[1].tv_sec = 0;
    ts[0].tv_nsec = ts[1].tv_nsec = UTIME_NOW;

    struct timespec tvp[2];
    struct tm *t;
    time_t now;
    now = time(NULL);
    t = localtime(&now);

    char *date = "201801102018";
    int i;
    int yearset = 0;

    switch(strlen(date)) {
        case 12:
            t->tm_year = ATOI2(date);
            t->tm_year *= 100;
        case 10:
            yearset = ATOI2(date);
            t->tm_year += yearset;

            t->tm_year -= 1900;
        case 8:
            t->tm_mon = ATOI2(date);
            --t->tm_mon;        /* Convert from 01-12 to 00-11 */
            t->tm_mday = ATOI2(date);
            t->tm_hour = ATOI2(date);
            t->tm_min = ATOI2(date);
            break;
    }

    t->tm_isdst = -1;
    tvp[0].tv_sec = tvp[1].tv_sec = mktime(t);
    tvp[0].tv_nsec = tvp[1].tv_nsec = 0;

    struct timeval tv[2];
    tv[0].tv_sec = tvp[0].tv_sec;
    tv[0].tv_usec = tvp[0].tv_nsec*1000;
    tv[1].tv_sec = tvp[1].tv_sec;
    tv[1].tv_usec = tvp[1].tv_nsec*1000;

    utimes( argv[ 1 ], tv );

/*
    int i = ATOI2(date);
    int j = ATOI2(date);
    int k = ATOI2(date);

    printf("%d\n", i);
    printf("%d\n", j);
    printf("%d\n", k);
*/
/*
    while ((c = getopt (argc, argv, "vabc:")) != -1)
        switch(c) {
            case 'a':
                printf("%c\n", c);
                break;
            case 'c':
                printf("%s\n", optarg);
                break;
            default:
                exit(0);
        }

    printf("%d\n", optind);

    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    //printf("%s\n", argv[2]);
    //printf("%s\n", argv[3]);
    //printf("%s\n", argv[5]);

    myname = basename(argv[ 1 ] );

    printf("%.*s\n", 3, myname);

    int res = stat(argv[1], &sb);

    printf("%s\n", strerror(errno));

    printf("%d\n", res);

    printf("%d - %d\n", errno, ENOENT);

    fd = open(argv[1], O_WRONLY | O_CREAT, DEFFILEMODE);

    if (fd == -1 || fstat(fd, &sb) || close(fd)) {
        warn("%s", *argv);
    }
*/

    return 1;
}