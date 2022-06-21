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

// readall: reads all bytes from the incomming stream, into a buffer returned in 's'
size_t readall(FILE *fp, char **s)
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

// dateseq: returns true if the date part (year, month, day) are equivalent
int dateseq(struct tm t1, struct tm t2)
{
    return t1.tm_year == t2.tm_year
        && t1.tm_mon == t2.tm_mon
        && t1.tm_mday == t2.tm_mday;
}

// cntdays: counts the days between the two dates
int cntdays(struct tm t1, struct tm t2, int weekends)
{
    int days = 0;

    while (!dateseq(t1, t2)) {
        if (weekends) {
            days++;
        } else {
            if (!(t1.tm_wday == 0 || t1.tm_wday == 6))
                days++;
        }

        t1.tm_mday++;
        mktime(&t1);
    }

    return days;
}

// cntlines: counts the lines in the null terminated strings
int cntlines(char *s)
{
    int lines = 0;
    while (*s) {
        if (*s == '\n')
            lines++;
        s++;
    }
    return lines + 1;
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

    days = cntdays(from, to, weekends);
    int seconds = 3600 * 8 * days; // compute seconds per task, assume 8 hours per day

    char *input = NULL;

    readall(stdin, &input);

    int lines = cntlines(input);
    int seconds_per_task = seconds / lines;

    char buf[BUFLARGE];
    curr = from;
    char *s = NULL;

    for (int i = 0; i < lines; i++) {
        int remaining_per_period = seconds_per_task;

        struct tm eod = curr;

        eod.tm_hour = 17; // EOD is 5pm
        eod.tm_min = 0;
        eod.tm_sec = 0;

        time_t t1 = mktime(&curr);
        time_t t2 = mktime(&eod);

        double diff = difftime(t1, t2);

        // NOTE (Brian) now that we have the difference between now and the EOD

        curr.tm_sec += seconds_per_task;
        mktime(&curr);

        s = strchr((s == NULL ? input : s), '\n');

        strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", &curr);

        printf("%s\t%s", buf, asctime(&curr));

        s++;
    }

    free(input);

    return 0;
}
