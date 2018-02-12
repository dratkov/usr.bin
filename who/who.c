#include <sys/stat.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <utmpx.h>
#include <paths.h>
#include <limits.h>
#include <langinfo.h>


static void quick(void);
static void whoami(void);
static void row(const struct utmpx *);


static int  qflag;          /* "Quick" mode */
static int  mflag;
static int  Tflag;          /* Show terminal state */
static int  uflag;          /* Show idle time */

int main(int argc, char *argv[])
{
    int ch;

    while ((ch = getopt(argc, argv, "HTabmqsu")) != -1) {
        switch(ch) {
            case 'q':
                qflag = 1;
                break;
            case 'm':
                mflag = 1;
                break;
            case 'T':       /* Show terminal state */
                Tflag = 1;
                break;
            case 'u':       /* Show idle time */
                uflag = 1;
                break;
        }
    }

    argc -= optind;
    argv += optind;

    if(argc >= 2 && strcmp(argv[0], "am") == 0 &&
       (strcmp(argv[1],"i") == 0 || strcmp(argv[1], "I") == 0)) {
        mflag = 1;
        argc -= 2;
        argv += 2;
    }

    whoami();
}

static void row(const struct utmpx *ut)
{
    char buf[80], tty[PATH_MAX];
    struct stat sb;
    time_t idle, t;
    static int d_first = -1;
    struct tm *tm;
    char state;

    if (d_first < 0)
        d_first = (*nl_langinfo(D_MD_ORDER) == 'd');

    state = '?';
    idle = 0;
    if (Tflag || uflag) {
        snprintf(tty, sizeof(tty), "%s%s", _PATH_DEV, ut->ut_line);
        if (stat(tty, &sb) == 0) {
            state = sb.st_mode & (S_IWOTH|S_IWGRP) ?
                '+' : '-';
            idle = time(NULL) - sb.st_mtime;
        }
    }

    printf("%-16s ", ut->ut_user);
    if (Tflag)
        printf("%c ", state);
    if (ut->ut_type == BOOT_TIME)
        printf("%-12s ", "system boot");
    else
        printf("%-12s ", ut->ut_line);
    t = ut->ut_tv.tv_sec;
    tm = localtime(&t);
    strftime(buf, sizeof(buf), d_first ? "%e %b %R" : "%b %e %R", tm);
    printf("%-*s ", 12, buf);
    if (uflag) {
        if (idle < 60)
            printf("  .   ");
        else if (idle < 24 * 60 * 60)
            printf("%02d:%02d ", (int)(idle / 60 / 60),
                                 (int)(idle / 60 % 60));
        else
            printf(" old  ");
    }
}

static void quick(void) {

}


static void whoami(void) {
    struct utmpx ut, *utx;
    struct passwd *pwd;
    const char *name, *tty;

    if ((tty = ttyname(STDIN_FILENO)) == NULL)
        tty = "tty??";
    else if (strncmp(tty, _PATH_DEV, sizeof _PATH_DEV - 1) == 0)
        tty += sizeof _PATH_DEV - 1;
    strlcpy(ut.ut_line, tty, sizeof ut.ut_line);

    if ((utx = getutxline(&ut)) != NULL && utx->ut_type == USER_PROCESS) {
        row(utx);
        return;
    }
}