#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"
#include "../io/binary_io.h"

#include "rrn.h"

int search_rrn()
{
    char bin_filename[100];
    int rrn;

    scanf("%s %d", bin_filename, &rrn);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if (bin_file == NULL){
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    Header *bin_header = read_binary_header(bin_file);

    if (bin_header == NULL)
        return MALLOC_ERROR;

    if (rrn < 0 || rrn >= bin_header->nextRRN)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }

    Record *result_record = read_rrn_record(bin_file, rrn);
    if (result_record == NULL)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }

    print_record(result_record);
    free(bin_header);
}