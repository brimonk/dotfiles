/*
 * Brian Chrzanowski
 * 2021-02-05 01:50:38
 *
 * Junk O Meter - Progressively Evaluate the Jankiness of Your Source Tree
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

#define REVIEW_FILE ("JUNKOMETER_REVIEWMSG")
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
	char *user;
	char *email;
	char *time;
	char *path;
	char *hash;
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
	int currfile;
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

// OpenEditor : opens the review message in the user's editor
int OpenEditor(char *s);
// ReadReviewMessage : reads the review message up to the first "^\s*#"
char *ReadReviewMessage(void);
// WriteReviewMessage : writes the template file
int WriteReviewMessage(char *path);
// RemoveReviewMessage : removes the template file
int RemoveReviewMessage(void);

// RunCommand : runs a command, and returns the result
char *RunCommand(char *cmd);

// GetRelativePath : returns a pointer to the relative path
char *GetRelativePath(char *path, char *base);

// GetCurrentTime : gets the current time
char *GetCurrentTime(void);
// GetGitCommit : returns the current git commit hash
char *GetGitCommit(void);
// GetGitUser : returns the current git user
char *GetGitUser(void);
// GetGitUser : returns the current git user
char *GetGitEmail(void);
// GetGitRoot : returns the full path of the current git repo
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
	char *root;
	char *absfile;
	char *relfile;
	char *s;
	char prompt[BUFLARGE];

#define PROMPT_PATTERN ("Reviewing: %s\nWhat now [n,p,r,v,s,q]? > ")

	root = GetGitRoot();
	absfile = state->files[state->currfile];
	relfile = GetRelativePath(absfile, root);

	snprintf(prompt, sizeof prompt, PROMPT_PATTERN, relfile);

	while ((s = readline(prompt))) {
		if (s[0] == 'q') {
			break;
		}

		if (strlen(s) == 0) {
			CMDHelp(state);
			continue;
		}

		// (1/1) Stage this hunk [y,n,q,a,d,e,?]?

		switch (s[0]) {
		case '?': // [?]
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

		free(s);

		absfile = state->files[state->currfile];
		relfile = GetRelativePath(absfile, root);
		snprintf(prompt, sizeof prompt, PROMPT_PATTERN, relfile);
	}

	if (s == NULL)
		printf("\n");

	free(root);

	return 0;
}

// CMDPrevFile : move the 'currfile' pointer back one
int CMDPrevFile(struct state_t *state)
{
	assert(state);

	state->currfile += 1;
	if (state->currfile < 0)
		state->currfile = state->files_len - 1;

	return 0;
}

// CMDNextFile : move the 'currfile' pointer forward one
int CMDNextFile(struct state_t *state)
{
	assert(state);

	state->currfile += 1;
	state->currfile %= state->files_len;

	return 0;
}

// CMDReview : edit the 'currfile'
int CMDReview(struct state_t *state)
{
	char *file;
	char *s;

	char *user;
	char *email;
	char *time;
	char *hash;
	char *comment;
	int score;

	// TODO (Brian) we need to make sure the user enters in an integer

	file = state->files[state->currfile];

	WriteReviewMessage(file);
	OpenEditor(REVIEW_FILE);

	comment = ReadReviewMessage();
	RemoveReviewMessage();

	s = readline("Score? > ");

	if (s == NULL) {
		printf("\n");
		exit(1);
	}

	score = atoi(s);

	user = GetGitUser();
	email = GetGitEmail();
	time = GetCurrentTime();
	hash = GetGitCommit();

	C_RESIZE(&state->ratings);

	state->ratings[state->ratings_len].user = user;
	state->ratings[state->ratings_len].email = email;
	state->ratings[state->ratings_len].time = time;
	state->ratings[state->ratings_len].path = strdup(file);
	state->ratings[state->ratings_len].hash = hash;
	state->ratings[state->ratings_len].comment = comment;
	state->ratings[state->ratings_len].score = score;

	state->ratings_len++;

	CMDNextFile(state);

	return 0;
}

// CMDViewFile : open the 'currfile' in the user's editor / pager
int CMDViewFile(struct state_t *state)
{
	char *file;

	file = state->files[state->currfile];
	OpenEditor(file);

	return 0;
}

// CMDHelp : help command
int CMDHelp(struct state_t *state)
{
	puts("n - next file");
	puts("s - score file");
	puts("p - prev file");
	puts("r - review file");
	puts("v - view file");

	puts("q - quit");

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
			state->ratings[state->ratings_len].user = v;
		} else if (strneq("AuthorEmail", k)) {
			state->ratings[state->ratings_len].email = v;
		} else if (strneq("Timestamp", k)) {
			state->ratings[state->ratings_len].time = v;
		} else if (strneq("Score", k)) {
			state->ratings[state->ratings_len].score = atoi(v);
			free(v);
		} else if (strneq("Path", k)) {
			state->ratings[state->ratings_len].path = v;
		} else if (strneq("Hash", k)) {
			state->ratings[state->ratings_len].hash = v;
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
	FILE *fp;
	int i;

	fp = fopen(DB_PATH, "wb");
	if (!fp) {
		return -1;
	}

	fprintf(fp, "%s\n", MARKER_NOTES);
	fprintf(fp, "%s\n", state->notes);
	fprintf(fp, "\n");

	fprintf(fp, "%s\n", MARKER_CONFIG);
	fprintf(fp, "ReviewGlob: %s\n", state->config.glob);
	fprintf(fp, "\n");

	for (i = 0; i < state->ratings_len; i++) {
		fprintf(fp, "%s\n", MARKER_REVIEW);

		fprintf(fp, "AuthorName: %s\n", state->ratings[i].user);
		fprintf(fp, "AuthorEmail: %s\n", state->ratings[i].email);
		fprintf(fp, "Timestamp: %s\n", state->ratings[i].time);
		fprintf(fp, "Score: %d\n", state->ratings[i].score);
		fprintf(fp, "Path: %s\n", state->ratings[i].path);
		fprintf(fp, "Hash: %s\n", state->ratings[i].hash);

		fprintf(fp, "\n");

		fprintf(fp, "%s\n", state->ratings[i].comment);

		fprintf(fp, "\n");
	}

	fclose(fp);

	return 0;
}

// FreeState : frees the state object
int FreeState(struct state_t *state)
{
	int i;

	assert(state);

	free(state->notes);

	for (i = 0; i < state->files_len; i++) {
		free(state->files[i]);
	}

	free(state->files);

	for (i = 0; i < state->ratings_len; i++) {
		free(state->ratings[i].user);
		free(state->ratings[i].email);
		free(state->ratings[i].time);
		free(state->ratings[i].path);
		free(state->ratings[i].hash);
		free(state->ratings[i].comment);
	}

	free(state->ratings);

	return 0;
}

// ReadReviewMessage : reads the review message up to the first "^\s*#"
char *ReadReviewMessage(void)
{
	char *s, *t;
	char *message;

	s = sys_readfile(REVIEW_FILE);
	t = strndup(s, strstr(s, "\n#") - s);

	message = strdup(trim(t));

	free(s);
	free(t);

	return message;
}

// OpenEditor : opens the review message in the user's editor
int OpenEditor(char *s)
{
	char tbuf[2048];

	snprintf(tbuf, sizeof tbuf, "$EDITOR %s", s != NULL ? s : "");
	system(tbuf);

	return 0;
}

// WriteReviewMessage : writes the template file
int WriteReviewMessage(char *path)
{
	FILE *f;

	f = fopen(REVIEW_FILE, "w+");
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
	remove(REVIEW_FILE);
	return 0;
}

// GetRelativePath : returns a pointer to the relative path
char *GetRelativePath(char *path, char *base)
{
	// NOTE (Brian) this totally doesn't work, unless path is already _in_ the
	// base. There's a way to write this, where it'll actually return ../s
	// and stuff, but that isn't this.
	//
	// You should do that at some point though. Seems like a good exercise.
	//
	// using strcmp for this is a hack

	while (path && base && *base && *path++ == *base++)
		;

	return path + 1;
}

// GetGitCommit : returns the current git commit hash
char *GetGitCommit(void)
{
	return RunCommand("git rev-parse HEAD");
}

// GetCurrentTime : gets the current time
char *GetCurrentTime(void)
{
	return RunCommand("date +\"%F %T %Z\"");
}

// GetGitUser : returns the current git user
char *GetGitUser(void)
{
	return RunCommand("git config user.name");
}

// GetGitUser : returns the current git user
char *GetGitEmail(void)
{
	return RunCommand("git config user.email");
}

// GetGitRoot : returns the full path of the current git repo
char *GetGitRoot(void)
{
	return RunCommand("git rev-parse --show-toplevel");
}

// RunCommand : runs a command, and returns the result
char *RunCommand(char *cmd)
{
	FILE *fp;
	char tbuf[BUFLARGE];
	char *s;

	fp = popen(cmd, "r");
	if (!fp) {
		ERR("Couldn't exec command: '%s'\n", cmd);
		return NULL;
	}

	fgets(tbuf, sizeof tbuf, fp);
	s = trim(tbuf);

	return strdup(s);
}

