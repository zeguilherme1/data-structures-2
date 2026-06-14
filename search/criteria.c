#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../constants.h"
#include "../models/header.h"
#include "../models/record.h"
#include "../models/index.h"
#include "../utils/utils.h"
#include "../io/io.h"
#include "./search.h"

// * Criteria and matching logic

void read_criteria(Search_criteria *criteria, int num_fields)
{
    // Iterate over the number of fields requested by the user
    for (int j = 0; j < num_fields; j++)
    {
        scanf("%s", criteria[j].field_name);

        // String fields need special reading because they can contain spaces and quotes
        if (strcmp(criteria[j].field_name, "nomeEstacao") == 0 ||
            strcmp(criteria[j].field_name, "nomeLinha") == 0)
        {
            scan_quote_string(criteria[j].field_value);
        }
        else
        {
            // Standard reading for integer fields (stored as strings initially)
            scanf("%s", criteria[j].field_value);
        }
    }
}

int matches_record_criteria(Record *rec, Search_criteria *criteria, int num_fields)
{
    // Check the record against every single criterion provided
    for (int i = 0; i < num_fields; i++)
    {
        int match = -1;

        // Route the match check to the correct field comparison function
        if (strcmp(criteria[i].field_name, "nomeEstacao") == 0)
            match = matches_string(criteria[i].field_value, rec->station_name, rec->station_name_size);

        else if (strcmp(criteria[i].field_name, "nomeLinha") == 0)
            match = matches_string(criteria[i].field_value, rec->line_name, rec->line_name_size);

        else if (strcmp(criteria[i].field_name, "codEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->station_code);

        else if (strcmp(criteria[i].field_name, "codLinha") == 0)
            match = matches_integer(criteria[i].field_value, rec->line_code);

        else if (strcmp(criteria[i].field_name, "codProxEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->next_station_code);

        else if (strcmp(criteria[i].field_name, "distProxEstacao") == 0)
            match = matches_integer(criteria[i].field_value, rec->next_station_distance);

        else if (strcmp(criteria[i].field_name, "codLinhaIntegra") == 0)
            match = matches_integer(criteria[i].field_value, rec->line_integration_code);

        else if (strcmp(criteria[i].field_name, "codEstIntegra") == 0)
            match = matches_integer(criteria[i].field_value, rec->station_integration_code);

        // If any criterion fails to match, the whole record is rejected immediately
        if (match != 0)
            return -1;
    }

    // Passed all criteria checks successfully
    return 0;
}

int get_station_code(Search_criteria *criteria, int num_fields)
{
    // Iterate to find if "codEstacao" was provided, as it is our primary key for indexed searches
    for (int i = 0; i < num_fields; i++)
    {
        if (strcmp(criteria[i].field_name, "codEstacao") == 0)
        {
            // Ensure it's not a NULL search string before converting
            if (strcmp(criteria[i].field_value, "NULO") == 0)
                return NO_DATA_ERROR;

            return atoi(criteria[i].field_value); // Convert the string criterion to int
        }
    }
    return NO_DATA_ERROR; // Return error code if the primary key field wasn't requested
}

// * Search strategies (sequential & indexed)

Record **sequential_search(FILE *fp, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    fseek(fp, data_offset, SEEK_SET); // Set file pointer to the start of the actual records

    Record **results = NULL;
    *count = 0;

    // Loop through the file sequentially until EOF is reached
    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(fp, temp_record);

        if (ret_record == -1) // EOF Reached
        {
            free_record(&temp_record);
            return results;
        }

        // Only consider valid records (skip logically removed ones)
        if (temp_record->removed == FALSE)
        {
            // Check if the current record satisfies all search filters
            if (matches_record_criteria(temp_record, criteria, num_fields) == 0)
            {
                // Reallocate the array to fit the newly found record
                results = realloc(results, (*count + 1) * sizeof(Record *));
                results[*count] = temp_record;
                (*count)++;
                continue; // Move to next iteration without freeing temp_record (it's saved in results)
            }
        }

        // Free the record from memory if it was removed or didn't match the criteria
        free_record(&temp_record);
    }
}

int find_rrn_by_station_code(FILE *index_file, int has_station_code)
{
    // Calculate total number of index entries
    fseek(index_file, 0, SEEK_END);
    long file_size = ftell(index_file);
    int total = (file_size - 1) / sizeof(PrimaryIndex); // Subtract header byte (status)

    int left = 0;
    int right = total - 1;

    // Jump past the index file status byte
    fseek(index_file, 1, SEEK_SET);

    // Execute a Binary Search since the index file is ordered by station_code
    while (left <= right)
    {
        int mid = (left + right) / 2;

        // Jump directly to the middle element's position
        fseek(index_file, 1 + mid * sizeof(PrimaryIndex), SEEK_SET);

        PrimaryIndex index;
        if (fread(&index, sizeof(PrimaryIndex), 1, index_file) != 1)
        {
            return NO_DATA_ERROR; // Read failed
        }

        if (index.station_code == has_station_code)
        {
            return index.rrn; // Found the matching station code, return its RRN
        }
        else if (index.station_code < has_station_code)
        {
            left = mid + 1; // The target is in the right half
        }
        else
        {
            right = mid - 1; // The target is in the left half
        }
    }

    return NO_DATA_ERROR; // Key not found after binary search completion
}

Record *indexed_search(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int has_station_code)
{
    if (has_station_code == NO_DATA_ERROR)
        return NULL;

    // First, find the RRN in the index file using binary search
    fseek(index_file, 0, SEEK_SET);
    int rrn = find_rrn_by_station_code(index_file, has_station_code);

    if (rrn == NO_DATA_ERROR)
        return NULL; // Key doesn't exist in the index

    // Retrieve the record directly from the data file using the calculated RRN
    Record *result_record = read_rrn_record(data_file, rrn);

    if (result_record == NULL)
        return NULL;

    // Check if it's a logically removed record
    if (result_record->removed == TRUE)
    {
        free_record(&result_record);
        return NULL;
    }

    // Even though the primary key matched, we must validate against the remaining criteria
    if (result_record->removed == FALSE &&
        matches_record_criteria(result_record, criteria, num_fields) == 0)
    {
        return result_record; // All criteria met
    }

    // Didn't meet the secondary criteria
    free_record(&result_record);
    return NULL;
}

Record **search_record(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    // Try extracting the station code to determine the optimal search strategy
    int has_station_code = get_station_code(criteria, num_fields);

    // If the index file is open and we have the primary key, prioritize Indexed Search (O(log n))
    if (has_station_code != NO_DATA_ERROR && index_file != NULL)
    {
        Record *rec = indexed_search(data_file, index_file, data_offset, criteria, num_fields, has_station_code);

        if (rec == NULL)
        {
            *count = 0;
            return NULL;
        }

        // Indexed search only returns max 1 record (Primary Key is unique)
        Record **results = malloc(sizeof(Record *));
        results[0] = rec;
        *count = 1;

        return results;
    }
    else
    {
        // Fallback to Sequential Search (O(n)) if no primary key is provided
        return sequential_search(data_file, data_offset, criteria, num_fields, count);
    }
}

// * High-level functionalities

int criteria_search()
{
    char bin_filename[100];
    scanf("%s", bin_filename); // Read target data file name

    int comparation_num;
    scanf("%d", &comparation_num); // Number of search queries to execute

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    // Read the file header to validate status and get offsets
    Header *temp_header = new_header();
    if (temp_header == NULL)
    {
        fclose(bin_file);
        return MALLOC_ERROR;
    }

    int ret_header = read_header(bin_file, temp_header);
    if (ret_header == -1)
    {
        free(temp_header);
        fclose(bin_file);
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    long data_offset = ftell(bin_file); // Save the position where the data records start

    // Loop through each search query requested
    for (int i = 0; i < comparation_num; i++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];
        read_criteria(criteria, num_fields); // Parse criteria fields

        int count = 0;
        // Trigger sequential scan for this specific query
        Record **rec = sequential_search(bin_file, data_offset, criteria, num_fields, &count);

        if (count == 0)
        {
            printf("Registro inexistente.\n"); // No matches found
        }
        else
        {
            // Print all matches found in the current query
            for (int j = 0; j < count; j++)
            {
                print_record(rec[j]);
                free_record(&rec[j]); // Free individual record to avoid memory leaks
            }
            free(rec); // Free the dynamic array itself
        }

        // Print blank line between query outputs, except for the last one
        if (i < comparation_num - 1)
            printf("\n");
    }

    free(temp_header);
    fclose(bin_file);

    return 0;
}

int index_or_criteria_search()
{
    char data_filename[100];
    char index_filename[100];

    // Read filenames for both data and index
    scanf("%s", data_filename);
    scanf("%s", index_filename);

    int comparation_num;
    scanf("%d", &comparation_num); // Number of searches

    FILE *data_file = fopen(data_filename, READ_BINARY_MODE);
    FILE *index_file = fopen(index_filename, READ_BINARY_MODE);

    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    // Process and validate Data Header
    Header *temp_header = new_header();
    if (temp_header == NULL)
    {
        fclose(data_file);
        fclose(index_file);
        return MALLOC_ERROR;
    }

    int ret_header = read_header(data_file, temp_header);
    if (ret_header == -1)
    {
        free(temp_header);
        fclose(data_file);
        fclose(index_file);
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    long data_offset = ftell(data_file); // Save start of records offset

    // Execute searches
    for (int i = 0; i < comparation_num; i++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];
        read_criteria(criteria, num_fields);

        int count = 0;
        // This abstraction decides if it will use index or sequential scanning
        Record **rec = search_record(data_file, index_file, data_offset, criteria, num_fields, &count);

        if (count == 0)
        {
            printf("Registro inexistente.\n");
        }
        else
        {
            for (int j = 0; j < count; j++)
            {
                print_record(rec[j]);
                free_record(&rec[j]);
            }
            free(rec);
        }

        if (i < comparation_num - 1)
        {
            printf("\n");
        }
    }

    fclose(data_file);
    fclose(index_file);
    free(temp_header);
    return SUCCESS;
}
