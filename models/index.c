#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "index.h"

#include "../io/binary_io.h"
#include "../utils/debug_utils.h"
#include "../constants.h"



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

int update_index_array(PrimaryIndex *index_array, int size, int rrn, int new_code)
{
    for(int i = 0; i < size; i++)
    {
        if(index_array[i]. rrn == rrn)
        {
            index_array[i].station_code = new_code;
            return SUCCESS;
        }
    }
    return NO_DATA_ERROR;
}

int compare_index(const void *a, const void *b)
{
    const PrimaryIndex *x = (const PrimaryIndex *)a;
    const PrimaryIndex *y = (const PrimaryIndex *)b;
    return x->station_code - y->station_code;
}

void rewrite_index_file(
    const char *index_filename,
    PrimaryIndex *indexes,
    int count
)
{
    FILE *index_file =
        fopen(index_filename, "wb+");

    if(index_file == NULL)
        return;

    char status = TRUE;

    fwrite(
        &status,
        sizeof(char),
        1,
        index_file
    );

    fwrite(
        indexes,
        sizeof(PrimaryIndex),
        count,
        index_file
    );

    fclose(index_file);
}
void remove_index_entry(
    PrimaryIndex *indexes,
    int *count,
    int station_code
)
{
    int pos = -1;

    for(int i = 0; i < *count; i++)
    {
        if(indexes[i].station_code
           == station_code)
        {
            pos = i;
            break;
        }
    }

    if(pos == -1)
        return;

    for(int i = pos;
        i < *count - 1;
        i++)
    {
        indexes[i] =
            indexes[i + 1];
    }

    (*count)--;
}
PrimaryIndex *load_indexes(
    FILE *index_file,
    int *count
)
{
    fseek(index_file, 0, SEEK_END);

    long size = ftell(index_file);

    *count =
        (size - 1)
        / sizeof(PrimaryIndex);

    PrimaryIndex *indexes =
        malloc(
            (*count)
            * sizeof(PrimaryIndex)
        );

    if(indexes == NULL)
        return NULL;

    fseek(index_file, 1, SEEK_SET);

    fread(
        indexes,
        sizeof(PrimaryIndex),
        *count,
        index_file
    );

    return indexes;
}