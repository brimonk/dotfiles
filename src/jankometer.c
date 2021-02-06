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
 */

#define COMMON_IMPLEMENTATION
#include "common.h"

#include <unistd.h>
#include <errno.h>

#define TEMPLATE_PATH ("JUNKOMETER_REVIEWMSG")
#define DB_PATH ("/home/brian/.jankdb")

// The section marker is enough characters to denote that the line of the
// file is a marker. Which marker it is is determined by the next three #defs.
#define MARKER_SECTION ("==========")
#define MARKER_NOTES   ("============== NOTES ==============")
#define MARKER_CONFIG  ("========== CONFIGURATION ==========")
#define MARKER_REVIEW  ("========== REVIEWED FILE ==========")

enum REPOTYPE {
	REPOTYPE_NONE,
	REPOTYPE_GIT,
	REPOTYPE_TOTAL
};

struct rating_t {
	char *author_name;
	char *author_email;
	char *timestamp;
	char *path;
	char *commit;
	char *comment;
	int score;
};

struct config_t {
	char *globs;
};

struct state_t {
	struct config_t config;
	char *notes;
	struct rating_t *ratings;
	size_t ratings_len, ratings_cap;
};

// ReviewLoop : this is the review loop
int ReviewLoop(struct state_t *state);

// ReadDB : reads the jankdb into state
int ReadDB(struct state_t *state);
// ReadDBConfig : reads the config section from the database
int ReadDBConfig(struct state_t *state, FILE *fp);
// ReadDBNotes : reads the notes section of the database
int ReadDBNotes(struct state_t *state, FILE *fp);
// ReadDBReview : reads the review section from the database
int ReadDBReview(struct state_t *state, FILE *fp);

// ReadDBToNextMarker : reads the database file up until the next marker, returns the string
char *ReadDBToNextMarker(FILE *fp);

// WriteDB : writes the jankdb from state
int WriteDB(struct state_t *state);

// FreeState : frees the state object
int FreeState(struct state_t *state);

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

	memset(&state, 0, sizeof state);

	ReadDB(&state);
	ReviewLoop(&state);
	WriteDB(&state);
	FreeState(&state);

#if 0
	WriteReviewMessage("junkometer.c");
	EditReviewMessage();
	s = ReadReviewMessage();
	printf("Message:\n%s\n", s);
	free(s);
	RemoveReviewMessage();
#endif

	return 0;
}

// ReviewLoop : this is the review loop
int ReviewLoop(struct state_t *state)
{
	return 0;
}

// ReadDB : reads the jankdb into state
int ReadDB(struct state_t *state)
{
	FILE *fp;
	char *s;
	char tbuf[BUFLARGE];

	fp = fopen(DB_PATH, "rb");
	if (!fp) {
		ERR("Couldn't open '%s'\n", DB_PATH);
		return -1;
	}

	while ((tbuf == fgets(tbuf, sizeof tbuf, fp))) {
		s = trim(tbuf);

		if (streq(s, MARKER_REVIEW)) {
			ReadDBReview(state, fp);
		} else if (streq(s, MARKER_NOTES)) {
			ReadDBNotes(state, fp);
		} else if (streq(s, MARKER_CONFIG)) {
			ReadDBConfig(state, fp);
		}
	}

	fclose(fp);

	return 0;
}

// ReadDBReview : reads the review section from the database
int ReadDBReview(struct state_t *state, FILE *fp)
{
	char tbuf[BUFLARGE];
	char *s, *k, *v;

	C_RESIZE(&state->ratings);

	while ((tbuf == fgets(tbuf, sizeof tbuf, fp))) {
		s = trim(tbuf);

		if (strlen(s) == 0) {
			break;
		}

		k = s;
		v = strchr(s, ':');

		if (v == NULL) { // potentially an error in the config section?
			continue;
		}

		v = strdup(++v);

		if (strneq("AuthorName", k)) {
			state->ratings[state->ratings_len].author_name = v;
		} else if (strneq("AuthorEmail", k)) {
			state->ratings[state->ratings_len].author_email = v;
		} else if (strneq("Timestamp", k)) {
			state->ratings[state->ratings_len].timestamp = v;
		} else if (strneq("Score", k)) {
			state->ratings[state->ratings_len].score = atoi(v);
			free(v);
		} else if (strneq("Path", k)) {
			state->ratings[state->ratings_len].path = v;
		} else if (strneq("Commit", k)) {
			state->ratings[state->ratings_len].commit = v;
		}
	}

	s = trim(ReadDBToNextMarker(fp));
	state->ratings[state->ratings_len].comment = s;

	printf("Path: %s\nComment:\n%s\n",
		state->ratings[state->ratings_len].path,
		state->ratings[state->ratings_len].comment);

	state->ratings_len++;

	return 0;
}

// ReadDBNotes : reads the notes section of the database
int ReadDBNotes(struct state_t *state, FILE *fp)
{
	char *s;

	s = ReadDBToNextMarker(fp);
	state->notes = s;

	return 0;
}

// ReadDBConfig : reads the config section from the database
int ReadDBConfig(struct state_t *state, FILE *fp)
{
	char tbuf[BUFLARGE];
	char *s, *k, *v;
	size_t tbuflen;

	while ((tbuf == fgets(tbuf, sizeof tbuf, fp))) {
		tbuflen = strlen(tbuf);
		s = trim(tbuf);

		if (strlen(s) == 0) { // config error?
			continue;
		}

		if (strneq(s, MARKER_SECTION)) {
			fseek(fp, -tbuflen, SEEK_CUR);
			break;
		}

		k = s;
		v = strchr(s, ':');

		if (v == NULL) { // potentially an error in the config section?
			continue;
		}

		v++;

		if (strneq(k, "ReviewGlobs")) {
			state->config.globs = strdup(v);
		}
	}

	return 0;
}

// ReadDBToNextMarker : reads the database file up until the next marker, returns the string
char *ReadDBToNextMarker(FILE *fp)
{
	char tbuf[BUFLARGE];
	char *s, *t;
	size_t s_len, s_cap;
	fpos_t pos;

	s = t = NULL;
	s_len = s_cap = 0;

	while ((tbuf == fgets(tbuf, sizeof tbuf, fp))) {
		t = tbuf;

		if (strneq(t, MARKER_SECTION)) { // rewind file pointer here
			fseek(fp, -strlen(t), SEEK_CUR);
			break;
		}

		if (s_cap < s_len + strlen(t)) {
			s_cap += sizeof tbuf;
			s = realloc(s, s_cap);
			s_len = strlen(s);
		}

		strncat(s, t, s_cap);
	}

	return s;
}

// WriteDB : writes the jankdb from state
int WriteDB(struct state_t *state)
{
	return 0;
}

// FreeState : frees the state object
int FreeState(struct state_t *state)
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

