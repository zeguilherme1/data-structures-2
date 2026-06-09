#ifndef CRITERIA_H
#define CRITERIA_H

#include <stdio.h>

typedef struct record Record;

//struct to store search fields
typedef struct criteria
{
    char field_name[50];
    char field_value[50];
} Search_criteria;

/*
    read_criteria:
        This function reads the n search criteria fields from input.
        It fills the criteria array with field names and their respective values.

        Args:
            - criteria: array of search criteria.
            - num_fields: number of fields to be read.

        Return:
            No return.
*/
void read_criteria(Search_criteria *criteria, int num_fields);

/*
    sequential_search:
        This function performs a sequential search in the data file.
        It reads each record and checks if it matches all given criteria.
        All matching records are stored in a dynamic array.

        Args:
            - fp: pointer to data file.
            - data_offset: byte offset where records start.
            - criteria: array of search criteria.
            - num_fields: number of criteria fields.
            - count: pointer to store number of found records.

        Return:
            Array of pointers to Records or NULL if none found.
*/
Record **sequential_search(FILE *fp, long data_offset, Search_criteria *criteria, int num_fields, int *count);

/*
    indexed_search:
        This function performs a search using the index file.
        It finds the RRN using binary search and retrieves the record from data file.
        The record is validated against all criteria.

        Args:
            - data_file: pointer to data file.
            - index_file: pointer to index file.
            - data_offset: byte offset where records start.
            - criteria: array of search criteria.
            - num_fields: number of criteria fields.
            - has_station_code: station code to search.

        Return:
            Pointer to the found Record or NULL if not found.
*/
Record *indexed_search(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int has_station_code);

/*
    search_record:
        This function decides which search strategy to use.
        If station code is present, indexed search is used.
        Otherwise, sequential search is performed.
        Results are returned in a dynamic array.

        Args:
            - data_file: pointer to data file.
            - index_file: pointer to index file (can be NULL).
            - data_offset: byte offset where records start.
            - criteria: array of search criteria.
            - num_fields: number of criteria fields.
            - count: pointer to store number of found records.

        Return:
            Array of pointers to Records or NULL if not found.
*/
Record **search_record(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int *count);

/*
    [functionality 3]
    criteria_search: 
        This function reads the file and performs multiple searches using criteria.
        For each search, it prints all matching records.
        If no record matches, a "not found" message is printed.

        Args:
            No args.

        Return:
            SUCCESS for success or FILE_NOT_FOUND for failure.
*/
int criteria_search();

/*
    get_station_code:
        This function extracts the station code from the criteria array.
        If the field is not present or is NULL, an error code is returned.

        Args:
            - criteria: array of search criteria.
            - num_fields: number of criteria fields.

        Return:
            Station code or NO_DATA_ERROR if not found.
*/
int get_station_code(Search_criteria *criteria, int num_fields);

/*
    find_rrn_by_station_code:
        This function performs a binary search on the index file.
        It finds the RRN corresponding to a given station code.
        
        Args:
            - index_file: pointer to index file.
            - has_station_code: station code to search.

        Return:
            RRN if found or NO_DATA_ERROR if not found.
*/
int find_rrn_by_station_code(FILE *index_file, int has_station_code);

/*
    [functionality 6]
    index_or_criteria_search:
        This function performs multiple searches using either index or sequential scan.
        If station code is provided, indexed search is used.
        Otherwise, sequential search is used.
        All matching records are printed.

        Args:
            No args.

        Return:
            SUCCESS for success or FILE_NOT_FOUND for failure.
*/
int index_or_criteria_search();

#endif