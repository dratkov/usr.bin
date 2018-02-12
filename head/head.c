#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

static void obsolete(char **);
static void head(FILE *, int);
static void head_bytes(FILE *, off_t);

int main(int argc, char *argv[])
{
	int ch;
	int first, linecnt = -1;
	off_t bytecnt = -1;
	char *ep;
	FILE *fp;

	obsolete(argv);
	while ((ch = getopt(argc, argv, "n:c:")) != -1)
		switch(ch) {
			case 'n':
				linecnt = strtol(optarg, &ep, 10);
				if (*ep || linecnt <= 0)
					errx(1, "illegal line count -- %s", optarg);
				break;
			case 'c':
				bytecnt = strtoimax(optarg, &ep, 10);
				if(*ep || bytecnt <= 0)
					errx(1, "illegal byte count -- %s", optarg);
				break;
		}

	argc -= optind;
	argv += optind;

	if (*argv) {
		for (first = 1; *argv; ++argv) {
			if ((fp = fopen(*argv, "r")) == NULL) {
				continue;
			}
			if(bytecnt == -1)
				head(fp, linecnt);
			else
				head_bytes(fp, bytecnt);
		}
	} else if(bytecnt == -1) {
		head(stdin, linecnt);
	} else {
		head_bytes(stdin, bytecnt);
	}


	return 0;
}

static void obsolete(char **argv)
{
	char *ap;

	while ((ap = *++argv)) {
		/* Return if "--" or not "-[0-9]*". */
		if (ap[0] != '-' || ap[1] == '-' || !isdigit(ap[1]))
			return;
		if ((ap = malloc(strlen(*argv) + 2)) == NULL)
			err(1, NULL);
		ap[0] = '-';
		ap[1] = 'n';
		(void)strcpy(ap + 2, *argv + 1);
		*argv = ap;
	}
}

static void head(FILE *fp, int cnt)
{
	char *cp;
	size_t error, readlen;

	while (cnt && (cp = fgetln(fp, &readlen)) != NULL) {
		error = fwrite(cp, sizeof(char), readlen, stdout);
		cnt--;
	}
}

static void head_bytes(FILE *fp, off_t cnt)
{
	char buf[4096];
	size_t readlen;

	while(cnt) {
		if(cnt < sizeof(buf)) {
			readlen = cnt;
		} else {
			readlen = sizeof(buf);
		}
		readlen = fread(buf, sizeof(char), readlen, fp);
		if(readlen == 0)
			break;
		if(fwrite(buf, sizeof(char), readlen, stdout) != readlen)
			err(1, "stdout");
		cnt -= readlen;
	}
}