#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct header
{
    char status;
    int top;
    int nextRRN;
    int station_num;
    int station_pairs_num;
} Header;

/*
    new_header:

    This function creates a new header and set up initial values

    Args:
        No args

    Return:
        (Header*) new_header: The generated header

*/
Header *new_header();

#endif
