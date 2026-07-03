#include <stdio.h>
#include <stdlib.h>

#include "header.h"
#include "../constants.h"

// * Memory management

Header *new_header()
{
    // Allocate memory dynamically for the new header structure
    Header *new_header = (Header *)malloc(sizeof(Header));

    // Safety check in case memory allocation fails
    if (new_header == NULL)
    {
        return NULL;
    }

    // Initialize base fields to default starting values for a new binary file
    new_header->status = TRUE;         // TRUE indicates the file is consistent/stable
    new_header->top = -1;              // -1 indicates the stack of logically removed records is empty
    new_header->nextRRN = 0;           // 0 means the next insertion will happen at the first position
    new_header->station_num = 0;       // Initialize the unique stations counter to 0
    new_header->station_pairs_num = 0; // Initialize the station pairs counter to 0

    return new_header; // Return the fully initialized header pointer
}