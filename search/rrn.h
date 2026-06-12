#ifndef RRN_H
#define RRN_H

#include <stdio.h> 

typedef struct record Record;
typedef struct criteria Search_criteria;

//struct to store a register and its rrn 
typedef struct {
    Record *record;
    int rrn;
} Search_result;


Search_result *search_with_rrn(
    FILE *data_file,
    FILE *index_file,
    long data_offset,
    Search_criteria *criteria,
    int num_fields,
    int *count
);

/*
    [functionality 4]
    search_rrn: 
        This function reads a binary file and an RRN, then retrieves and prints
        the corresponding record.

        It validates if the RRN is within bounds and if the record exists.
        If the record is invalid or not found, a "not found" message is printed.

        Args:
            No args.

        Return:
            SUCCESS for success, FILE_NOT_FOUND if file cannot be opened,
            or NO_DATA_ERROR if the record does not exist.

*/
int search_rrn();
void remove_record_by_rrn(FILE *data_file, Header *header, int rrn);
#endif