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

void dump_css()
{
	puts("<style>");
	puts("* {box-sizing:border-box}");
	puts("");
	puts("/* Slideshow container */");
	puts(".slideshow-container {");
	puts("  max-width: 1000px;");
	puts("  position: relative;");
	puts("  margin: auto;");
	puts("}");
	puts("");
	puts("/* Hide the images by default */");
	puts(".slides {");
	puts("  display: none;");
	puts("}");
	puts("");
	puts("/* Next & previous buttons */");
	puts(".prev, .next {");
	puts("  cursor: pointer;");
	puts("  position: absolute;");
	puts("  top: 50%;");
	puts("  width: auto;");
	puts("  margin-top: -22px;");
	puts("  padding: 16px;");
	puts("  color: white;");
	puts("  font-weight: bold;");
	puts("  font-size: 18px;");
	puts("  transition: 0.6s ease;");
	puts("  border-radius: 0 3px 3px 0;");
	puts("  user-select: none;");
	puts("}");
	puts("");
	puts("/* Position the 'next button' to the right */");
	puts(".next {");
	puts("  right: 0;");
	puts("  border-radius: 3px 0 0 3px;");
	puts("}");
	puts("");
	// puts("/* On hover, add a black background color with a little bit see-through */");
	// puts(".prev:hover, .next:hover {");
	// puts("  background-color: rgba(0,0,0,0.8);");
	// puts("}");
	puts("");
	puts("/* Caption text */");
	puts(".text {");
	puts("  color: #f2f2f2;");
	puts("  font-size: 15px;");
	puts("  padding: 8px 12px;");
	puts("  position: absolute;");
	puts("  bottom: 8px;");
	puts("  width: 100%;");
	puts("  text-align: center;");
	puts("}");
	puts("");
	puts("/* Number text (1/3 etc) */");
	puts(".numbertext {");
	puts("  color: #f2f2f2;");
	puts("  font-size: 12px;");
	puts("  padding: 8px 12px;");
	puts("  position: absolute;");
	puts("  top: 0;");
	puts("}");
	puts("");
	// puts(".active, .dot:hover {");
	// puts("  background-color: #717171;");
	// puts("}");
	puts("");
	puts("/* Fading animation */");
	puts(".fade {");
	puts("  animation-name: fade;");
	puts("  animation-duration: 1.5s;");
	puts("}");
	puts("");
	puts("@keyframes fade {");
	puts("  from {opacity: .4}");
	puts("  to {opacity: 1}");
	puts("}");
	puts("</style>");
}

// dump_js: dumps out javascript for use in controlling the slideshow
void dump_js()
{
	puts("<script>");
	puts("let slide = 0;");
	puts("");
	puts("function clamp(n, a, b) {");
	puts("    if (n < a)");
	puts("        return a;");
	puts("    if (n > b)");
	puts("        return b;");
	puts("    return n;");
	puts("}");
	puts("");
	puts("function next() {");
	puts("    showslides(slide += 1);");
	puts("}");
	puts("");
	puts("function prev() {");
	puts("    showslides(slide -= 1);");
	puts("}");
	puts("");
	puts("function showslides(n) {");
	puts("    let i;");
	puts("    let slides = document.getElementsByClassName('slides');");
	puts("    n = clamp(n, 0, slides.length - 1);");
	puts("    slide = n;");
	puts("");
	puts("    for (i = 0; i < slides.length; i++)");
	puts("        slides[i].style.display = 'none';");
	puts("");
	puts("    slides[slide].style.display = 'block';");
	puts("    slides[slide].className += ' active';");
	puts("}");
	puts("");
	puts("document.addEventListener('keydown', (event) => {");
	puts("    let key = event.key;");
	puts("    const map = {");
	puts("        ' ': next");
	puts("        , 'ArrowLeft': prev");
	puts("        , 'ArrowRight': next");
	puts("    };");
	puts("    if (map[key])");
	puts("        map[key]();");
	puts("}, false);");
	puts("");
	puts("showslides(slide);");
	puts("</script>");
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
    int z;

    if (argc < 3) {
        freopen(argv[1], "rb", stdin);
    }

	memset(&gstate, 0, sizeof gstate);

    printf("<!DOCTYPE html>\n");
    printf("<html lang=\"en-US\">\n");

	puts("<header>");
	dump_css();
	puts("</header>");

    printf("<body>\n");

	puts("<div class=\"slideshow-container\">");

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

		if ((z = is_header(s))) { // everything is in the same slide until the next one, or we end
			if (slide++) {
				printf("</div>");
			}
			printf("<div class=\"slides fade\">");
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

	puts("</div>"); // finish last slide we saw

	puts("</div>"); // slideshow-container

	dump_js();

    printf("</body>\n");

    printf("</html>\n");

    return 0;
}
