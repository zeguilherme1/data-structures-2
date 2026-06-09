#ifndef RRN_H
#define RRN_H

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

#endif