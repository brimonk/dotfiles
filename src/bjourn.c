// Brian Chrzanowski
// 2022-04-16 14:46:24
//
// Copyright (c) 2022
//
// Basically, a Bullet Journaling Program
//
// USAGE:
//
// Command line interfacing is similar to using git, and making commits. Effectively, every
// invocation either directly modifies the file from the command, or we dump some text into a
// temporary file, have the user edit it manually (with the git commented out thing)
//
// SYMBOLS:
//
//   T task
//   E events
//   N notes
//
//   I important (combo)
//
//   C completed things get an 'x'
//   R an important item, move to next month
//   M if the task is due in the future, migrate this into the future
//
// TODO:
//
// - task completion
// - task deferral
// - task migration
// - important tasks sort to the top (without ignoring whitespace)
// - save with dates for new lines added
// - do not show user dates on reload
// - diffing
// - show only a timeslice
// - case insensitivity for commands
// - previous history
// - task warning / duration detection (write in the real file the date the line was written)

#define COMMON_IMPLEMENTATION
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <pwd.h>

#define FLAG_VIEW_DAY    (0x01)
#define FLAG_VIEW_MONTH  (0x02)
#define FLAG_VIEW_YEAR   (0x04)

#define DEFAULT_BJOURN_LOCATION ("$HOME/.bjourn")

unsigned int flags = 0;

void strtoupper(char *s)
{
    for (; *s; s++)
        *s = toupper(*s);
}

// PrintUsage: prints usage information into the file
void PrintUsage(char *fname)
{
    struct tm curr;
    time_t t;
    char tbuf[BUFSMALL];

	FILE *fp = fopen(fname, "a");
	if (fp == NULL) {
		return;
	}

	fprintf(fp, "# Make any changes you see fit to your bullet journal.\n");
	fprintf(fp, "# When you are done making changes, simply save the file.\n");
	fprintf(fp, "# Lines starting with '#' will be ignored. If you change your\n");
	fprintf(fp, "# mind, simply save the file without any changes.\n");
	fprintf(fp, "#\n");
	fprintf(fp, "# Commands:\n");
	fprintf(fp, "#\tT: task\n");
	fprintf(fp, "#\tE: event\n");
	fprintf(fp, "#\tN: note\n");
	fprintf(fp, "#\tI: important\n");
	fprintf(fp, "#\tC: completed\n");
	fprintf(fp, "#\tD: defer this item for next month\n");
	fprintf(fp, "#\tM: migrate this item for the future month specified\n");
	fprintf(fp, "#\n");
	fprintf(fp, "# Next Week Dates:\n");

    t = time(NULL);

    for (curr = *localtime(&t); curr.tm_wday != 1; mktime(&curr)) {
        curr.tm_mday++;
    }

    while (1 <= curr.tm_wday && curr.tm_wday <= 5) {
        strftime(tbuf, sizeof tbuf, "%Y-%m-%d %a", &curr);
        strtoupper(tbuf);

        fprintf(fp, "# %s\n", tbuf);

        curr.tm_mday++;
        mktime(&curr);
    }

	fclose(fp);
}

int DoTheThing()
{
	char path[PATH_MAX];
	char cmd[PATH_MAX * 2];

	strcpy(path, "/tmp/tmp.XXXXXX");

	int fd = mkstemp(path);

	if (fd == -1) { // error
		return -1;
	}

	close(fd);

	snprintf(cmd, sizeof cmd, "grep -v '^#' < \"%s\" > \"%s\"", DEFAULT_BJOURN_LOCATION, path);
	system(cmd);

	PrintUsage(path);

	snprintf(cmd, sizeof cmd, "$EDITOR \"%s\"", path);
	system(cmd);

	snprintf(cmd, sizeof cmd, "grep -v '^#' < \"%s\" > \"%s\"", path, DEFAULT_BJOURN_LOCATION);
	system(cmd);

	return 0;
}

int main(int argc, char **argv)
{
	int rc;

	rc = DoTheThing();
	if (rc < 0) {
		return 1;
	}

	return 0;
}

