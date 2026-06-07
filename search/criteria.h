#ifndef CRITERIA_H
#define CRITERIA_H

typedef struct criteria
{
    char field_name[50];
    char field_value[50];
} Search_criteria;

void read_criteria(Search_criteria *criteria, int num_fields);
int sequential_scan_with_criteria(FILE *fp, long data_offset, Search_criteria *criteria, int num_fields);
int criteria_search();
int get_station_code(Search_criteria *criteria, int num_fields);
int find_rrn_by_station_code(FILE *index_file, int has_station_code);
int index_or_criteria_search();

#endif