#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "index.h"

#include "../io/binary_io.h"
#include "../utils/debug_utils.h"
#include "../constants.h"

int compare_index(const void *a, const void *b)
{
    const PrimaryIndex *x = (const PrimaryIndex *)a;
    const PrimaryIndex *y = (const PrimaryIndex *)b;

    return x->station_code - y->station_code;
}

void create_index_file()
{
    char bin_filename[100], primary_index_bin[100];

    scanf("%s %s", bin_filename, primary_index_bin);

    FILE *data_file = fopen(bin_filename, READ_BINARY_MODE);
    FILE *index_file = fopen(primary_index_bin, WRITE_BINARY_MODE);

    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        if (data_file)
            fclose(data_file);
        if (index_file)
            fclose(index_file);
        return;
    }

    Header *header = read_binary_header(data_file);

    if (header == NULL || header->status != TRUE)
    {
        printf("Falha no processamento do arquivo.");
        fclose(data_file);
        fclose(index_file);
        free(header);
        return;
    }

    PrimaryIndex *indexes = malloc(header->nextRRN * sizeof(PrimaryIndex));

    if (indexes == NULL)
    {
        printf("Falha no processamento do arquivo.");
        fclose(data_file);
        fclose(index_file);
        free(header);
        return;
    }

    int index_count = 0;
    int rrn = 0;

    while (true)
    {
        Record *tmp_record = new_record();

        int ret = read_record(data_file, tmp_record);

        if (ret == -1)
        {
            free_record(&tmp_record);
            break;
        }

        if (tmp_record->removed == FALSE)
        {
            indexes[index_count].station_code =
                tmp_record->station_code;

            indexes[index_count].rrn = rrn;

            index_count++;
        }

        rrn++;

        free_record(&tmp_record);
    }

    qsort(indexes, index_count, sizeof(PrimaryIndex), compare_index);

    char status = '0';

    fwrite(&status, sizeof(char), 1, index_file);

    for (int i = 0; i < index_count; i++)
    {
        fwrite(&indexes[i].station_code,
               sizeof(int),
               1,
               index_file);

        fwrite(&indexes[i].rrn,
               sizeof(int),
               1,
               index_file);
    }

    status = '1';

    rewind(index_file);

    fwrite(&status, sizeof(char), 1, index_file);

    free(indexes);
    free(header);

    fclose(data_file);
    fclose(index_file);

    BinarioNaTela(primary_index_bin);
}