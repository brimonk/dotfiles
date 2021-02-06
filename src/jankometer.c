/*
 * Brian Chrzanowski
 * 2021-02-05 01:50:38
 *
 * Junk O Meter
 *
 * Commands
 *
 * - [c]omment
 * - [e]valuate
 * - [r]eload database
 * - [s]kip file
 * - [v]iew file
 * - [w]rite database
 * - [q]uit
 *
 * TODO (Brian) Main Program
 * 1. iterate over the files that follow a specific expression
 * 2. order to view files we haven't evaluated yet (in eval mode)
 * 3. when we get to an unreviewed file, it opens the file in the editor, and when it closes we ask for the eval, just like git add -p
 * 4. when we get to a reviewed file, same thing, just show the last review
 */

#define COMMON_IMPLEMENTATION
#include "common.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <regex.h>

#define TEMPLATE_PATH ("JUNKOMETER_REVIEWMSG")
#define DB_PATH ("/home/brian/.jankdb") // TODO where do we load these from?

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
	char *glob;
};

struct state_t {
	struct config_t config;
	char *notes;
	struct rating_t *ratings;
	size_t ratings_len, ratings_cap;
	char **files;
	size_t files_len, files_cap;
};

// ReviewLoop : this is the review loop
int ReviewLoop(struct state_t *state);

// CMDHelp : help command
int CMDHelp(struct state_t *state);
// CMDPrevFile : move the 'currfile' pointer back one
int CMDPrevFile(struct state_t *state);
// CMDNextFile : move the 'currfile' pointer forward one
int CMDNextFile(struct state_t *state);
// CMDReview : edit the 'currfile'
int CMDReview(struct state_t *state);
// CMDViewFile : open the 'currfile' in the user's editor / pager
int CMDViewFile(struct state_t *state);

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

// FindReviewableFiles : find all reviewable files in the current repo
int FindReviewableFiles(struct state_t *state, char *path);

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

// GetGitRoot : changes current working directory to the git root
char *GetGitRoot(void);

int main(int argc, char **argv)
{
	struct state_t state;

	memset(&state, 0, sizeof state);

	ReadDB(&state);
	ReviewLoop(&state);
	WriteDB(&state);
	FreeState(&state);

	return 0;
}

// ReviewLoop : this is the review loop
int ReviewLoop(struct state_t *state)
{
	char *s;

	while ((s = readline("What now? > "))) {
		if (s[0] == 'q') {
			break;
		}

		switch (s[0]) {
		case 'h': // [h]elp
			CMDHelp(state);
			break;

		case 'p': // [p]rev
			CMDPrevFile(state);
			break;

		case 'r': // [r]eview
			CMDReview(state);
			break;

		case 'v': // [v]iew
			CMDViewFile(state);
			break;

		case 'n': // [n]ext
		case 's': // [s]kip
			CMDNextFile(state);
			break;

		default:
			printf("Unknown command: '%s'\n", s);
			CMDHelp(state);
			break;
		}
	}

	if (s == NULL)
		printf("\n");

	return 0;
}

// CMDPrevFile : move the 'currfile' pointer back one
int CMDPrevFile(struct state_t *state)
{
	return 0;
}

// CMDNextFile : move the 'currfile' pointer forward one
int CMDNextFile(struct state_t *state)
{
	return 0;
}

// CMDReview : edit the 'currfile'
int CMDReview(struct state_t *state)
{

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

// CMDViewFile : open the 'currfile' in the user's editor / pager
int CMDViewFile(struct state_t *state)
{
	return 0;
}

// CMDHelp : help command
int CMDHelp(struct state_t *state)
{
	puts("*** Commands ***");

	puts("[e]valuate");
	puts("[n]ext file");
	puts("[p]rev file");
	puts("[r]eview");
	puts("[s]kip file");
	puts("[v]iew file");

	puts("[q]uit");

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

	FindReviewableFiles(state, NULL);

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

		v += 2;

		if (strneq(k, "ReviewGlob")) {
			state->config.glob = strdup(v);
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

// FindReviewableFiles : find all reviewable files in the current repo
int FindReviewableFiles(struct state_t *state, char *path)
{
	DIR *d;
	struct dirent *dir;
	regex_t preg;
	regmatch_t pmatch[16];
	int rc;
	char *s, *t;
	char tbuf[BUFLARGE];

	// TODO (Brian) There's something weird with the posix regex matching in
	// the file checking. A quick run of it seems to suggest that a match will
	// have rm_so == 0, and rm_eo != 0, so that's what we're going with for the
	// moment

	if (path == NULL) { // base case
		s = GetGitRoot();
		assert(s);
		rc = FindReviewableFiles(state, s);
		free(s);
		return rc;
	}

	rc = regcomp(&preg, state->config.glob, REG_EXTENDED);
	if (rc != 0) {
		regerror(rc, &preg, tbuf, sizeof tbuf);
		ERR("Couldn't compile the regular expression [%s]: %s\n", state->config.glob, tbuf);
		regfree(&preg);
		return -1;
	}

	d = opendir(path);
	if (d == NULL) {
		printf("ERROR PATH '%s'\n", path);
		perror("opendir"); // What's the halfway decent way of dealing with this?
		exit(1);
	}

	while ((dir = readdir(d)) != NULL) {
		C_RESIZE(&state->files);

		memset(pmatch, 0, sizeof pmatch);

		if (streq(dir->d_name, ".") || streq(dir->d_name, "..")) {
			continue;
		}

		snprintf(tbuf, sizeof tbuf, "%s/%s", path, dir->d_name);
		s = strdup(tbuf);
		t = strdup(tbuf);

		switch (dir->d_type) {
			case DT_REG: { // the other base case
				rc = regexec(&preg, t, sizeof pmatch, pmatch, 0);
				if (pmatch[0].rm_so == 0 && pmatch[0].rm_eo != 0) {
					state->files[state->files_len++] = s;
				}
				break;
			}

			case DT_DIR: { // the recursive case
				FindReviewableFiles(state, tbuf);
				break;
			}
		}
	}

	closedir(d);

	regfree(&preg);

	return 0;
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

// GetGitRoot : changes current working directory to the git root
char *GetGitRoot(void)
{
	// TODO (Brian) return -1 if we aren't in a git repo
	FILE *fp;
	char tbuf[BUFLARGE];
	char *s;

	fp = popen("git rev-parse --show-toplevel", "r");
	if (!fp) {
		ERR("Couldn't get git root directory!\n");
		return NULL;
	}

	fgets(tbuf, sizeof tbuf, fp);
	s = trim(tbuf);

	return strdup(s);
}

