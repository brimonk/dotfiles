#!/usr/bin/env bash
# NOTE (Brian)
# Going to be honest, I don't really understand why I wrote it like this. We could have just created
# a nice little table, and had the functions within that table, but NO. NEVER.
#
# You should really change it to do this, when you're reading this in the future.

RUNFILE="${0%.sh}.c"

rm -f ${RUNFILE%.c} *.c Makefile

algorithms=(
	"bubble"
	"selection"
	"insertion"
	"quick"
	"merge"
)

cat << EOF >> $RUNFILE
// Brian Chrzanowski
// $(date +'%Y-%m-%d %H:%M:%S')
//
// Run 'make' and run the output binary, and observe the output :)
// Happy speedrunning!
//
// To determine overall time, subtract the end time in the binary, and there you go.
//
// You are allowed to use the already written swap function.

void swap(int *a, int *b)
{
	int z = *a;
	*a = *b;
	*b = z;
}

EOF

for func in "${algorithms[@]}"; do
cat << EOF >> $RUNFILE
// $func
void $func(int *arr, int len)
{
}

EOF
done

cat << EOF >> $RUNFILE
struct functab {
	char *name;
	void (*func)(int *, int);
};

struct functab FUNCTAB[] = {
EOF

for func in "${algorithms[@]}"; do
cat << EOF >> $RUNFILE
	{ "$func", $func },
EOF
done

cat << EOF >> $RUNFILE
};

EOF

cat << EOF >> $RUNFILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/time.h>

#define RED "\\033[1;31m"
#define GRN "\\033[1;32m"
#define RST "\\033[0m"

#define ARRSIZE 8
#define LOGFILE "run.txt"
#define ARRLEN(x) (sizeof((x)) / sizeof((x)[0]))
#define STARTTIME $(date +'%s.%N')

int intcmp(const void *a, const void *b)
{
	int ia = *(int *)a;
	int ib = *(int *)b;

	if (ia < ib)
		return -1;
	else if (ia > ib)
		return 1;
	return 0;
}

// getrandarr: writes a random array of length 'len' into 'arr'
int getrandarr(int *arr, int len)
{
	for (int i = 0; i < len; i++)
		arr[i] = rand();
	return 0;
}

// logtrial: logs
int logtrial(char *func, int isgood)
{
	FILE *fp = fopen(LOGFILE, "a");
	char buf[64];
	time_t t;
	struct tm *now;

	t = time(NULL);
	now = localtime(&t);

	strftime(buf, sizeof buf, "%F %T", now);
	fprintf(fp, "%c %s %s\n", isgood ? 'P' : 'F', buf, func);

	fclose(fp);

	return 0;
}

int main(int argc, char **argv)
{
	int std[ARRSIZE];
	int arr[ARRSIZE];
	int rc, i, successes;

	srand(time(NULL));

	printf(GRN "P - PASS" RST "\n");
	printf(RED "F - FAIL" RST "\n");
	printf("--------\n");

	successes = 0;

	for (i = 0; i < ARRLEN(FUNCTAB); i++) {
		memset(std, 0, sizeof std);
		memset(arr, 0, sizeof arr);

		getrandarr(std, ARRSIZE);

		memcpy(arr, std, sizeof arr);
		qsort(std, ARRSIZE, sizeof(std[0]), intcmp);

		FUNCTAB[i].func(arr, ARRSIZE);

		rc = memcmp(std, arr, sizeof std) == 0;
		if (rc) {
			printf(GRN "P - %s" RST "\n", FUNCTAB[i].name);
			successes++;
		} else {
			printf(RED "F - %s" RST "\n", FUNCTAB[i].name);
		}

		logtrial(FUNCTAB[i].name, rc);
	}

	if (successes == ARRLEN(FUNCTAB)) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		double end = tv.tv_sec + (tv.tv_usec * 0.000001);
		printf(GRN "SUCCESS! %lf" RST "\n", end - STARTTIME);
	}

	return 0;
}
EOF

cat << EOF >> Makefile
all: *.c
	\$(CC) -g3 -Wall -o ${RUNFILE%.c} \$^
EOF

make
