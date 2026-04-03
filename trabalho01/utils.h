#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

int csv_to_bin();
int bin_to_text();
int integer_or_null(char* str);
void ScanQuoteString(char *str);
int criteria_search();

typedef struct criteria {
	char field_name[50];
    char field_value[50];
} Search_criteria;

#endif
