#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>
#include "../models/header.h"
#include "../models/record.h"
#include "../io/io.h"

/*
    exists_station_name:
        This function scans the data file sequentially to check if a
        given station name already exists among the active (not removed) records.

        Args:
            - data_file: pointer to the binary data file.
            - header: pointer to the current file header.
            - name: string containing the station name to search for.

        Return:
            1 if the station name exists, 0 otherwise.
*/
int exists_station_name(FILE *data_file, Header *header, const char *name);

/*
    exists_station_pair:
        This function scans the data file sequentially to check if a
        specific pair of station code and next station code already exists.

        Args:
            - data_file: pointer to the binary data file.
            - header: pointer to the current file header.
            - code: the source station code.
            - next_code: the destination station code.

        Return:
            1 if the pair exists, 0 otherwise.
*/
int exists_station_pair(FILE *data_file, Header *header, int code, int next_code);

/*
    delete_records:
        This function handles the logical deletion of records based on user-provided
        search criteria. It updates the removal stack in the header, decrements unique
        counters if necessary, and removes the corresponding entries from the index file.

        Args:
            No args.

        Return:
            SUCCESS (0) on success, or FILE_NOT_FOUND on failure.
*/
int delete_records();

/*
    read_string_field:
        This function reads a string input (handling quotes) and correctly
        allocates memory for it. It handles the specific "NULO" keyword
        by setting the destination pointer to NULL and size to 0.

        Args:
            - dest: double pointer to dynamically store the string.
            - size: pointer to store the resulting string size.

        Return:
            No return.
*/
void read_string_field(char **dest, int *size);

/*
    read_insert_record:
        This function reads all fields required to create a new Record from the
        standard input, handling "NULO" cases, and returns a fully populated Record.

        Args:
            No args.

        Return:
            Pointer to the newly created Record, or NULL on failure.
*/
Record *read_insert_record();

/*
    insert_records:
        This function inserts multiple new records into the binary file. It prioritizes
        reusing logically removed space (managed by the header's top stack) before
        appending to the end of the file. It also updates the index file.

        Args:
            No args.

        Return:
            SUCCESS (0) on success, or FILE_NOT_FOUND on failure.
*/
int insert_records();

/*
    apply_updates:
        This function iterates through an array of update criteria and modifies
        the fields of a target Record accordingly.

        Args:
            - rec: pointer to the Record to be updated.
            - updates: array containing the fields and their new values.
            - p: the number of update criteria provided.

        Return:
            No return.
*/
void apply_updates(Record *rec, Search_criteria *updates, int p);

/*
    update_records:
        This function searches for records matching specific criteria and applies
        updates to them. It overwrites the updated records back into the binary file
        and safely maintains the primary index array.

        Args:
            No args.

        Return:
            SUCCESS (0) on success, or FILE_NOT_FOUND on failure.
*/
int update_records();

#endif