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
 * TODO (Brian) check if programs are on the computer (which?)
 *
 * TODO (Brian) Iterate Through Files
 *
 * ============ REVIEWED FILE ================
 * path: path/in/tree/from/git/root
 * average_score: 3.4
 *
 * (ends without a closing thing)
 */

#define COMMON_IMPLEMENTATION
#include "common.h"

#include <unistd.h>
#include <errno.h>

#define TEMPLATE_PATH ("JUNKOMETER_REVIEWMSG")
#define DB_PATH ("$HOME/.jankdb")

enum REPOTYPE {
	REPOTYPE_NONE,
	REPOTYPE_GIT,
	REPOTYPE_TOTAL
};

struct rating_t {
	char *author_name;
	char *author_email;
	char *comment;
	struct tm tm;
	int score;
	char *path_from_root;
	char *commit;
};

struct config_t {
	char *globs;
};

struct state_t {
	struct config_t config;
	MK_RESIZE_ARR(struct rating_t, ratings);
};

// EditReviewMessage : opens the review message in the user's editor
int EditReviewMessage(void);
// ReadReviewMessage : reads the review message up to the first "^\s*#"
char *ReadReviewMessage(void);
// WriteReviewMessage : writes the template file
int WriteReviewMessage(char *path);
// RemoveReviewMessage : removes the template file
int RemoveReviewMessage(void);

int main(int argc, char **argv)
{
	struct state_t state;
	char *s;

	WriteReviewMessage("junkometer.c");
	EditReviewMessage();
	s = ReadReviewMessage();
	printf("Message:\n%s\n", s);
	free(s);
	RemoveReviewMessage();

	return 0;
}

// ReadDB : reads the jankdb into state
int ReadJank(struct state_t *state)
{
	return 0;
}

// WriteDB : writes the jankdb from state
int WriteDB(struct state_t *state)
{
	return 0;
}

// ReadReviewMessage : reads the review message up to the first "^\s*#"
char *ReadReviewMessage(void)
{
	char *s, *t;
	char *message;

	s = sys_readfile(TEMPLATE_PATH);
	t = strndup(s, strstr(s, "\n#") - s);

	message = strdup(trim(t));

	free(s);
	free(t);

	return message;
}

// EditReviewMessage : opens the review message in the user's editor
int EditReviewMessage(void)
{
	char tbuf[2048];

	snprintf(tbuf, sizeof tbuf, "$EDITOR %s", TEMPLATE_PATH);
	system(tbuf);

	return 0;
}

// WriteReviewMessage : writes the template file
int WriteReviewMessage(char *path)
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

// RemoveReviewMessage : removes the template file
int RemoveReviewMessage(void)
{
	remove(TEMPLATE_PATH);
	return 0;
}

// CDToGitRoot : changes current working directory to the git root
int CDToGitRoot(void)
{
	// TODO (Brian) return -1 if we aren't in a git repo
	return 0;
}

