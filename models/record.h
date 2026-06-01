#ifndef RECORD_H
#define RECORD_H

#include "../search/criteria.h"

typedef struct record
{
    char removed;

    int next_record;

    int station_code;
    int line_code;

    int next_station_code;
    int next_station_distance;

    int line_integration_code;
    int station_integration_code;

    int station_name_size;
    char *station_name;

    int line_name_size;
    char *line_name;

} Record;

typedef struct
{
    int station_code;
    int next_station_code;
} Pair;

Record *new_record();

void free_record(Record **record);

void print_record(Record *record);

int matches_record_criteria(
    Record *record,
    Search_criteria *criteria,
    int num_fields
);

#endif