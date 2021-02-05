/*
 * Brian Chrzanowski
 * 2021-02-05 01:50:38
 *
 * Junk O Meter
 *
 * TODO (Brian) Main Program
 * 1. iterate over the files that follow a specific expression
 * 2. order to view files we haven't evaluated yet (in eval mode)
 * 3. when we get to an unreviewed file, it opens the file in the editor, and when it closes we ask for the eval, just like git add -p
 * 4. when we get to a reviewed file, same thing, just show the last review
 *
 * TODO (Brian) DB Format
 *
 * I think the file format should probably look something like this:
 *
 * TODO (Brian) Iterate Through Files
 *
 * ============ REVIEWED FILE ================
 * path: path/in/tree/from/git/root
 * average_score: 3.4
 *
 * (ends without a closing thing)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <unistd.h>
#include <errno.h>

#define TEMPLATE_PATH ("JUNKOMETER_REVIEWMSG")

struct rating_t {
	char *author_name;
	char *author_email;
	char *comment;
	int value;
};

struct file_t {
	char *path;
};

// WriteTemplate : writes the template file
int WriteTemplate(char *path);
// RemoveTemplate : removes the template file
int RemoveTemplate(void);

int main(int argc, char **argv)
{
	char tbuf[2048];

	WriteTemplate("junkometer.c");

	snprintf(tbuf, sizeof tbuf, "$EDITOR %s", TEMPLATE_PATH);
	system(tbuf);

	snprintf(tbuf, sizeof tbuf, "cat %s", TEMPLATE_PATH);
	system(tbuf);

	RemoveTemplate();

	return 0;
}

// WriteTemplate : writes the template file
int WriteTemplate(char *path)
{
	FILE *f;

	f = fopen(TEMPLATE_PATH, "w+");
	if (!f) {
		return -1;
	}

	fprintf(f, "\n");
	fprintf(f, "# Write your review message above these lines.\n");
	fprintf(f, "# Reviewing: %s\n", path);
	fprintf(f, "\n");

	fclose(f);

	return 0;
}

// RemoveTemplate : removes the template file
int RemoveTemplate(void)
{
	remove(TEMPLATE_PATH);
	return 0;
}

// CDToGitRoot : changes current working directory to the git root
int CDToGitRoot(void)
{
	// todo fill me out, probably need to use popen or something
	return 0;
}

