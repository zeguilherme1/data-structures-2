#ifndef STRING_UTILS_H
#define STRING_UTILS_H

int integer_or_null(char *str);

void print_int(int value);

void print_string(char *str, int size);

int matches_integer(
    char *criteria,
    int field
);

int matches_string(
    char *criteria,
    char *field,
    int size
);

void clean_string(char *str);

#endif