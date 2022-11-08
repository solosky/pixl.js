#include <stdio.h>
#include <string.h>

#include "m-string.h"

#define MAX_COLUMN 80

static void format(FILE *f)
{
    string_t s;
    string_init(s);
    while (string_fgets(s, f, STRING_READ_PURE_LINE) == true)
    {
        size_t n = string_size(s);
        if (n > 0 && string_get_char(s, n-1) == '\\' && n < MAX_COLUMN) {
            string_left(s, n-1);
            while (n < MAX_COLUMN-1) {
                string_push_back(s, ' ');
                n++;
            }
            string_push_back(s, '\\');
        }
        if (n >= MAX_COLUMN && string_get_char(s, n-1) == '\\') {
            n--;
            string_left(s, n);
            while (n >= MAX_COLUMN-2 && string_get_char(s, n-1) == ' ') {
                n--;
                string_left(s, n);
            }
            string_push_back(s, ' ');
            string_push_back(s, '\\');
        }
        printf("%s\n", string_get_cstr(s));
    }
    string_clear(s);
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR: Missing argument filename.\nUSAGE: %s filename.h\n", argv[0]);
        exit(1);
    }
    FILE *f = fopen(argv[1], "rt");
    if (!f) {
        fprintf(stderr, "ERROR: Cannot open filename '%s' .h\n", argv[1]);
        exit(2);
    }
    format(f);
    fclose(f);
    exit(0);
}
