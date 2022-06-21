// Brian Chrzanowski
// 2022-05-09 17:13:49
//
// My Slideshow Program
//
// This program will read in a file marked down with a markdown-like syntax, and produce a
// completely standalone slideshow contained within a single HTML source file. This file can then
// be transmitted anywhere, and loaded into a browser for viewing pleasure.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

struct state {
	int isbold;
	int isital;
	int isuline;
};

struct state gstate;

// ltrim: trims whitespace from the left of the string
char *ltrim(char *s)
{
    while (isspace(*s))
        s++;
    return s;
}

// rtrim: trims whitespace from the right of the string
char *rtrim(char *s)
{
    char *e;
    for (e = s + strlen(s) - 1; isspace(*e); e--)
        *e = 0;
    return s;
}

// trim: calls both ltrim and rtrim
char *trim(char *s) { return ltrim(rtrim(s)); }

void styles();

void header()
{
    printf("<head>\n");
    styles();
    printf("</head>\n");
}

// styles: print styles out
void styles()
{
    printf("<style>\n");
    printf("</style>\n");
}

// is_header: returns greater than 0, for what header level this is, if it's a header
int is_header(char *s)
{
    int level = 0;
    while (*s++ == '#')
        level++;
    return level;
}

// is_olist: returns true if this line is part of an ordered list
int is_olist(char *s)
{
    // search for the pattern, '\d*'
    while (*s != '.')
        if (!isdigit(*s))
            return false;
    return true;
}

// is_ulist: returns true if this line is part of an unordered list
int is_ulist(char *s)
{
    return (*s == '*' || *s == '-' || *s == '+') && *(s + 1) == ' ';
}

// is_code: returns true if this line is code
int is_code(char *s)
{
    return strcmp(s, "```") == 0 || strcmp(s, "    ") == 0;
}

// dump_js: dumps out javascript for use in controlling the slideshow
void dump_js()
{
}

int write_str(char *s)
{
	char *t;

	for (t = s; *s; s++) {
		switch (*s) {
			case '\\': // escape
				break;
			case '*': // bold / ital
				if (*(s + 1) && *(s + 1) == '*') { // bold
					if (gstate.isbold) {
						printf("</b>");
					} else {
						printf("<b>");
					}
					gstate.isbold = !gstate.isbold;
					// increment 's', we basically already consumed another character
					s++;
				} else { // ital
					if (gstate.isital) {
						printf("</i>");
					} else {
						printf("<i>");
					}
					gstate.isital = !gstate.isital;
				}
				break;
			case '_': // underline
				if (gstate.isuline) {
					printf("</u>");
				} else {
					printf("<u>");
				}
				gstate.isuline = !gstate.isuline;
				break;
			default:
				putchar(*s);
		}
	}

	return s - t;
}

// base64e: base64 encode
void base64e()
{
}

int main(int argc, char **argv)
{
    size_t len;
    char buf[1024];
    char *s;
    int slide = 0;
    int inol = false;
    int inul = false;
    int incode = false;
    int prev;
    int z;

    if (argc < 3) {
        freopen(argv[1], "rb", stdin);
    }

    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"en-US\">\n");

    header();

    printf("<body>\n");

	memset(&gstate, 0, sizeof gstate);

    while (buf == fgets(buf, sizeof buf, stdin)) {
        len = strlen(buf);

        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        s = trim(buf);

		if (strlen(s) == 0) {
			if (inol) {
				inol = false;
				printf("</ol>\n");
				continue;
			}
			if (inul) {
				inul = false;
				printf("</ul>\n");
				continue;
			}
		}

		if (is_code(s)) {
			if (incode) {
				printf("</pre></code>\n");
			} else {
				printf("<code><pre>");
			}
			incode = !incode;
			continue;
		} else {
			if (incode) {
				printf("%s\n", buf); // maintain whitespace by using 'buf'
				continue;
			}
		}

        z = is_header(s);
        if (0 < z) {
            printf("<h%d>%s</h%d>\n", z, s + z + 1, z);
            continue;
        }

		if (is_olist(s)) {
			if (!inol) {
				printf("<ol>\n");
				inol = true;
			}
			printf("<li>");
			write_str(s + 2);
			printf("</li>");
			continue;
		} else {
			if (inol) {
				printf("</ol>\n");
				inol = false;
			}
		}

		if (is_ulist(s)) {
			if (!inul) {
				printf("<ul>\n");
				inul = true;
			}
			printf("<li>");
			write_str(s + 2);
			printf("</li>");
			continue;
		} else {
			if (inul) {
				printf("<ul>\n");
				inul = false;
			}
		}

		write_str(s);
    }

    printf("</body>\n");

	dump_js();

    printf("</html>\n");

    return 0;
}
