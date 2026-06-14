#ifndef RECORD_H
#define RECORD_H

#include "../search/search.h"

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

/*
    new_record:
        This function dynamically allocates memory for a new Record structure
        and initializes all its fields to default safety values (-1 for integers,
        NULL for pointers, and FALSE for boolean flags).

        Args:
            No args.

        Return:
            Pointer to the newly allocated Record, or NULL if memory allocation fails.
*/
Record *new_record();

/*
    free_record:
        This function frees all dynamically allocated memory associated with a Record,
        including its internal strings (station_name and line_name), and sets the
        original pointer to NULL to prevent dangling pointers.

        Args:
            - record: double pointer to the Record to be freed.

        Return:
            No return.
*/
void free_record(Record **record);

/*
    print_record:
        This function prints the fields of a given record to the standard output,
        formatting integer and string fields appropriately, handling NULL representations.

        Args:
            - record: pointer to the Record to be printed.

        Return:
            No return.
*/
void print_record(Record *record);

/*
    read_rrn_record:
        This function calculates the byte offset based on the Relative Record Number (RRN)
        and reads the corresponding record directly from the binary file. It handles
        both fixed-size fields and variable-size dynamic strings.

        Args:
            - bin_file: pointer to the binary data file.
            - rrn: the Relative Record Number indicating the record's position.

        Return:
            Pointer to the populated Record, or NULL if the record is logically removed
            or if reading fails.
*/
Record *read_rrn_record(FILE *bin_file, int rrn);

#endif