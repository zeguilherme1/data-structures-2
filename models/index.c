#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "index.h"

#include "../io/io.h"
#include "../utils/utils.h"
#include "../constants.h"


// * File creation and rewriting


void create_index_file()
{
    char bin_filename[100], primary_index_bin[100];

    // Read the source data file name and the target index file name
    scanf("%s %s", bin_filename, primary_index_bin);

    FILE *data_file = fopen(bin_filename, READ_BINARY_MODE);
    FILE *index_file = fopen(primary_index_bin, WRITE_BINARY_MODE);

    // Validate if files were opened successfully
    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        if (data_file)
            fclose(data_file);
        if (index_file)
            fclose(index_file);
        return;
    }

    // Read the main data file header
    Header *header = read_binary_header(data_file);

    // Ensure the data file is consistent/stable
    if (header == NULL || header->status != TRUE)
    {
        printf("Falha no processamento do arquivo.");
        fclose(data_file);
        fclose(index_file);
        free(header);
        return;
    }

    // Allocate memory for the index array based on the total possible records
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

    // Scan the entire data file to build the index mappings
    while (true)
    {
        Record *tmp_record = new_record();

        int ret = read_record(data_file, tmp_record);

        if (ret == -1) // Reached EOF
        {
            free_record(&tmp_record);
            break;
        }

        // Only index valid (not logically removed) records
        if (tmp_record->removed == FALSE)
        {
            indexes[index_count].station_code = tmp_record->station_code;
            indexes[index_count].rrn = rrn;
            index_count++;
        }

        rrn++; // Keep tracking RRN regardless of whether the record was removed

        free_record(&tmp_record);
    }

    // Sort the entire index array by station code in ascending order
    qsort(indexes, index_count, sizeof(PrimaryIndex), compare_index);

    // Mark index file status as '0' (inconsistent) while writing
    char status = '0';
    fwrite(&status, sizeof(char), 1, index_file);

    // Write all sorted indexes sequentially to the binary file
    for (int i = 0; i < index_count; i++)
    {
        fwrite(&indexes[i].station_code, sizeof(int), 1, index_file);
        fwrite(&indexes[i].rrn, sizeof(int), 1, index_file);
    }

    // Process finished successfully, mark file as '1' (consistent)
    status = '1';
    rewind(index_file);
    fwrite(&status, sizeof(char), 1, index_file);

    // Clean up resources
    free(indexes);
    free(header);

    fclose(data_file);
    fclose(index_file);

    // Execute testing validation function
    BinarioNaTela(primary_index_bin);
}

void rewrite_index_file(const char *index_filename, PrimaryIndex *indexes, int count)
{
    // Open the index file in write-binary-plus mode to truncate and overwrite
    FILE *index_file = fopen(index_filename, "wb+");

    if(index_file == NULL)
        return;

    // Write the consistent status flag at the start of the file
    char status = TRUE;
    fwrite(&status, sizeof(char), 1, index_file);

    // Bulk write the entire array in memory directly into the file
    fwrite(indexes, sizeof(PrimaryIndex), count, index_file);

    fclose(index_file);
}


// * Array manipulation and sorting


int update_index_array(PrimaryIndex *index_array, int size, int rrn, int new_code)
{
    // Iterate through the array to find the exact RRN
    for(int i = 0; i < size; i++)
    {
        if(index_array[i].rrn == rrn)
        {
            // Found it, apply the new station code and return success
            index_array[i].station_code = new_code;
            return SUCCESS;
        }
    }
    // RRN was not found in the index array
    return NO_DATA_ERROR;
}

int compare_index(const void *a, const void *b)
{
    // Cast void pointers to PrimaryIndex types
    const PrimaryIndex *x = (const PrimaryIndex *)a;
    const PrimaryIndex *y = (const PrimaryIndex *)b;
    
    // Return difference to define sort order for qsort
    return x->station_code - y->station_code;
}

void remove_index_entry(PrimaryIndex *indexes, int *count, int station_code)
{
    int pos = -1;

    // Search linearly for the position of the target station code
    for(int i = 0; i < *count; i++)
    {
        if(indexes[i].station_code == station_code)
        {
            pos = i;
            break;
        }
    }

    // If it wasn't found, there is nothing to remove
    if(pos == -1)
        return;

    // Shift all subsequent elements one position to the left to overwrite the removed entry
    for(int i = pos; i < *count - 1; i++)
    {
        indexes[i] = indexes[i + 1];
    }

    // Decrement the total tracker to reflect the removal
    (*count)--;
}

PrimaryIndex *load_indexes(FILE *index_file, int *count)
{
    // Seek to the end of the file to determine its byte size
    fseek(index_file, 0, SEEK_END);
    long size = ftell(index_file);

    // Calculate total index elements (discounting 1 byte for the status header)
    *count = (size - 1) / sizeof(PrimaryIndex);

    // Allocate exact memory needed for the array
    PrimaryIndex *indexes = malloc((*count) * sizeof(PrimaryIndex));

    if(indexes == NULL)
        return NULL;

    // Skip the status byte and read the entire index structure into memory
    fseek(index_file, 1, SEEK_SET);
    fread(indexes, sizeof(PrimaryIndex), *count, index_file);

    return indexes;
}

void insert_index_sorted(PrimaryIndex **indexes, int *count, int *capacity, int station_code, int rrn)
{
    // Check if the array reached its memory limits and needs resizing
    if (*count >= *capacity)
    {
        *capacity *= 2; // Double the capacity strategy
        *indexes = realloc(*indexes, (*capacity) * sizeof(PrimaryIndex));
    }

    // Insert the new element at the current end of the array
    (*indexes)[*count].station_code = station_code;
    (*indexes)[*count].rrn = rrn;
    (*count)++;

    // Re-sort the array immediately to maintain index integrity
    qsort(*indexes, *count, sizeof(PrimaryIndex), compare_index);
}