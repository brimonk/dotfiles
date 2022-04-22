// Brian Chrzanowski
// 2022-04-22 09:41:56
//
// Take a list of tasks, and print dates and times when they should possibly be delivered to stay
// on track for a final delivery date.
//
// TODO
// - Take an Optional End Date from the User
// - Take an Optional Start Date from the User
// - Take an Optional Parmeter to Count Weekends
// - Perform the time tracking with seconds. The results will look a little weird, but it'll be
//   most accurate.

#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFLARGE (0x01 << 12)

// ReadFromStream: reads all bytes from the incomming stream, into a buffer returned in 's'
size_t ReadFromStream(FILE *fp, char **s)
{
    size_t cap = 0;
    size_t len = 0;

    while (!(feof(fp) || ferror(fp))) {
        if (len >= cap) {
            cap += BUFLARGE;
            *s = realloc(*s, cap);
            memset((*s) + len, 0, cap - len);
        }

        len += fread((*s) + len, 1, cap - len, fp);
    }

    return len;
}

int main(int argc, char **argv)
{
    struct tm curr, from, to;
    time_t t;
    int days = 0;
    int weekends = 0;

    memset(&from, 0, sizeof from);
    memset(&to, 0, sizeof to);
    memset(&curr, 0, sizeof curr);

    t = time(NULL);
    from = *localtime(&t);

    strptime("2022-05-31 17:00:00", "%Y-%m-%d %H:%M:%S", &to);

    // NOTE We really want to count the hours. Count the days first because that number * 8 will be
    // how many working hours you have.

    for (days = 0, curr = from; mktime(&curr) <= mktime(&to); curr.tm_mday++) {
        while (!weekends && (curr.tm_wday == 0 || curr.tm_wday == 6)) {
            curr.tm_mday++;
            mktime(&curr);
        }
        printf("Curr: %s", asctime(&curr));
        days++;
    }

    int totalhours = days * 8;
    char *input = NULL;

    ReadFromStream(stdin, &input);

    int lines = 0;
    char *s = NULL;

    while ((s = strchr((s == NULL ? input : s), '\n')) != NULL)
        lines++, s++;

    int hourspertask = totalhours / lines;
    s = NULL;

    char buf[BUFLARGE];
    curr = from;

    for (int i = 0; i < lines; i++) {
        // determine the delivery date and time
        curr.tm_hour += hourspertask;
        mktime(&curr);

        // get the task to complete from the line
        s = strchr((s == NULL ? input : s), '\n');

        strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &curr);

        printf("%s\t%s", buf, asctime(&curr));

        s++;
    }

    free(input);

    return 0;
}
