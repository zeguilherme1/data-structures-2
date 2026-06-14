#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"
#include "../io/io.h"

#include "./search.h"

Search_result *search_with_rrn(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    *count = 0;
    int capacity = 10; // Start with a default capacity to avoid constant reallocation
    Search_result *results = malloc(capacity * sizeof(Search_result));

    // Try finding the primary key to optimize the search
    int has_station_code = get_station_code(criteria, num_fields);

    // Optimized indexed search path (If we have codEstacao)
    if (has_station_code != NO_DATA_ERROR && index_file != NULL)
    {
        fseek(index_file, 0, SEEK_SET);
        int rrn = find_rrn_by_station_code(index_file, has_station_code);

        if (rrn == NO_DATA_ERROR)
            return NULL; // Not found

        Record *rec = read_rrn_record(data_file, rrn);

        // Validate if the record exists, is active, and respects the remaining criteria
        if (rec != NULL &&
            rec->removed == FALSE &&
            matches_record_criteria(rec, criteria, num_fields) == 0)
        {
            // Found matching record, allocate strictly what is needed
            results = realloc(results, sizeof(Search_result));
            results[0].record = rec;
            results[0].rrn = rrn;
            *count = 1;
            return results;
        }

        // Discard the record if it didn't pass the final validation
        if (rec != NULL)
            free_record(&rec);
        free(results);
        return NULL;
    }

    // Fallback: Sequential search path
    fseek(data_file, data_offset, SEEK_SET);

    int rrn = 0;

    // Scan through the entire data file
    while (1)
    {
        Record *rec = new_record();
        int ret = read_record(data_file, rec);

        if (ret == -1) // EOF reached
        {
            free_record(&rec);
            break;
        }

        // Validate if the record is active and satisfies all criteria
        if (rec->removed == FALSE &&
            matches_record_criteria(rec, criteria, num_fields) == 0)
        {
            // Capacity check: Double the memory size if we ran out of space in the array
            if (*count == capacity)
            {
                capacity = (capacity == 0) ? 10 : capacity * 2;

                Search_result *temp = realloc(results, capacity * sizeof(Search_result));

                // Safety check in case memory allocation fails
                if (temp == NULL)
                {
                    for (int i = 0; i < *count; i++)
                        free_record(&results[i].record);

                    free(results);
                    free_record(&rec);
                    return NULL;
                }

                results = temp;
            }

            // Store the matching record and its respective RRN
            results[*count].record = rec;
            results[*count].rrn = rrn;
            (*count)++;
            rrn++;
            continue; // Skip the free() at the end of the loop since we kept the reference
        }

        // Clean up memory if it was removed or didn't match
        free_record(&rec);
        rrn++;
    }

    return results;
}

int search_rrn()
{
    char bin_filename[100];
    int rrn;

    scanf("%s %d", bin_filename, &rrn); // Read file name and the target RRN to fetch

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if (bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    Header *bin_header = read_binary_header(bin_file);

    if (bin_header == NULL)
        return MALLOC_ERROR;

    // Validate bounds: The requested RRN must be positive and lower than the total inserted records
    if (rrn < 0 || rrn >= bin_header->nextRRN)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }

    // Direct access in O(1) to the exact byte offset using RRN calculation
    Record *result_record = read_rrn_record(bin_file, rrn);

    if (result_record == NULL)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }

    print_record(result_record); // Display fetched record
    free(bin_header);
    return SUCCESS;
}

void remove_record_by_rrn(FILE *data_file, Header *header, int rrn)
{
    // Calculate precise byte offset using standard equation: Header Size + (RRN * Record Size)
    long offset = HEADER_SIZE + rrn * RECORD_SIZE;

    fseek(data_file, offset, SEEK_SET);

    // Read the first byte to verify if it's already removed
    char removed_flag;
    fread(&removed_flag, sizeof(char), 1, data_file);
    if (removed_flag == TRUE)
        return; // Ignore if already removed

    // Go back to the beginning of the record to start overwriting
    fseek(data_file, offset, SEEK_SET);

    char removed = TRUE;
    int old_top = header->top; // Get the current Top of the logical removal stack

    // Mark the record as removed
    fwrite(&removed, sizeof(char), 1, data_file);

    // Write the old top to maintain the linked stack of logically removed records
    fwrite(&old_top, sizeof(int), 1, data_file);

    // Update the header's top variable to point to the newly removed RRN (Push operation)
    header->top = rrn;
}