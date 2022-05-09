// Brian Chrzanowski
// 2022-05-09 17:13:49
//
// My Slideshow Program
//
// This program will read in a file marked down with a markdown-like syntax, and produce a
// completely standalone slideshow contained within a single HTML source file. This file can then
// be transmitted anywhere, and loaded into a browser for viewing pleasure.
//
// The document structure is somewhat related to 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

// is_listo: returns true if this line is part of an ordered list
int is_listo(char *s)
{
    // search for the pattern, '\d*'
    while (*s != '.')
        if (!isdigit(*s))
            return false;
    return true;
}

// is_listu: returns true if this line is part of an unordered list
int is_listu(char *s)
{
    return *s == '*';
}

// is_code: returns true if this line is code
int is_code(char *s, int incode)
{
    int lineterm = strcmp(s, "```") == 0;
    int codeindent = strcmp(s, "    ") == 0;

    if (codeindent) {
        return true;
    } else if (lineterm && incode) {
        return false;
    } else if (lineterm && !incode) {
        return true;
    } else {
        return false;
    }

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

    if (argc == 2) {
        freopen(argv[1], "rb", stdin);
    }

    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"en-US\">\n");

    header();

    printf("<body>\n");

    while (buf == fgets(buf, sizeof buf, stdin)) {
        len = strlen(buf);

        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        prev = incode;
        incode = is_code(s, incode);

        if (incode && incode == prev) {
            printf("%s\n", buf);
        } else if (incode && !prev) {
            printf("<code><pre>");
        } else if (!incode && prev) {
            printf("</pre></code>");
        }

        s = trim(buf);

        z = is_header(s);
        if (0 < z) {
            printf("<h%d>%s</h%d>\n", z, s + z + 1, z);
            continue;
        }

        printf("%s\n", buf);
    }

    printf("</body>\n");

    printf("</html>\n");

    return 0;
}
