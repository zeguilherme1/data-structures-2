#include <stdio.h>
#include "header.h"
#include "../constants.h"
#include "header.h"

Header *new_header()
{

    Header *new_header = (Header *)malloc(sizeof(Header));

    if (new_header == NULL)
    {
        return NULL;
    }

    new_header->status = TRUE;
    new_header->top = -1;
    new_header->nextRRN = 0;
    new_header->station_num = 0;
    new_header->station_pairs_num = 0;

    return new_header;
}
