#ifndef CRITERIA_H
#define CRITERIA_H

typedef struct criteria
{
    char field_name[50];
    char field_value[50];
} Search_criteria;

int criteria_search();

#endif