#ifndef INDEX_H
#define INDEX_H

#include <stdio.h>

typedef struct
{
    int station_code;
    int rrn;
} PrimaryIndex;

/*
    create_index_file:
        This function reads the main data file and creates a corresponding
        primary index file. The index maps station codes to their respective RRNs,
        is sorted in ascending order of the station code, and outputs a hash
        using BinarioNaTela for validation.

        Args:
            No args.

        Return:
            No return.
*/
void create_index_file();

/*
    update_index_array:
        This function searches for a specific RRN within an index array and
        updates its associated station code.

        Args:
            - index_array: array of primary indexes.
            - size: number of elements in the array.
            - rrn: target Relative Record Number to be updated.
            - new_code: the new station code to overwrite the old one.

        Return:
            SUCCESS if the entry was found and updated, NO_DATA_ERROR otherwise.
*/
int update_index_array(PrimaryIndex *index_array, int size, int rrn, int new_code);

/*
    compare_index:
        This function acts as a comparator for the qsort function. It compares
        two PrimaryIndex structures based on their station_code field.

        Args:
            - a: pointer to the first PrimaryIndex.
            - b: pointer to the second PrimaryIndex.

        Return:
            Negative if a < b, 0 if a == b, positive if a > b.
*/
int compare_index(const void *a, const void *b);

/*
    rewrite_index_file:
        This function overwrites the entire index file with the contents
        of the current index array in memory. It safely sets the file status
        to TRUE upon completion.

        Args:
            - index_filename: string containing the name of the index file.
            - indexes: array of primary indexes to be written.
            - count: number of elements in the index array.

        Return:
            No return.
*/
void rewrite_index_file(const char *index_filename, PrimaryIndex *indexes, int count);

/*
    remove_index_entry:
        This function logically removes an entry from the index array in memory
        by finding the target station code and shifting subsequent elements left.

        Args:
            - indexes: array of primary indexes.
            - count: pointer to the total number of elements (will be decremented).
            - station_code: the station code of the entry to be removed.

        Return:
            No return.
*/
void remove_index_entry(PrimaryIndex *indexes, int *count, int station_code);

/*
    load_indexes:
        This function reads the entire index file into a dynamically allocated
        array in memory for fast access and manipulation.

        Args:
            - index_file: pointer to the opened binary index file.
            - count: pointer to store the number of loaded index entries.

        Return:
            Pointer to the dynamically allocated array of PrimaryIndex, or NULL on failure.
*/
PrimaryIndex *load_indexes(FILE *index_file, int *count);

/*
    insert_index_sorted:
        This function inserts a new index entry into the index array. It handles
        dynamic memory reallocation if the array is full and ensures the array
        remains sorted by station code after insertion.

        Args:
            - indexes: double pointer to the array of primary indexes.
            - count: pointer to the current number of elements in the array.
            - capacity: pointer to the maximum capacity of the array.
            - station_code: the station code of the new entry.
            - rrn: the RRN of the new entry.

        Return:
            No return.
*/
void insert_index_sorted(PrimaryIndex **indexes, int *count, int *capacity, int station_code, int rrn);

#endif