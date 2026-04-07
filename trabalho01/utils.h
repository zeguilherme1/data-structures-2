#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define TRUE '1'
#define FALSE '0'
#define SUCCESS 0
#define FILE_NOT_FOUND -1
#define MALLOC_ERROR -2
#define NO_DATA_ERROR -3

typedef struct criteria {
    char field_name[50];
    char field_value[50];
} Search_criteria;

void BinarioNaTela(char *arquivo);
int integer_or_null(char *str);
int csv_to_bin();
int bin_to_text();
void scan_quote_string(char *str);
int criteria_search();
int search_rrn();

#endif
