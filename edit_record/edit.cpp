
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"
#include "../search/search.h"
#include "../models/index.h"
#include "../utils/utils.h"
#include "../io/io.h"

// * Validation helpers

static int exists_station_name(FILE *data_file, Header *header, const char *name)
{
    // Start searching right after the header
    fseek(data_file, HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < header->nextRRN; i++)
    {
        Record *rec = new_record();

        // Break the loop if we hit EOF unexpectedly
        if (read_record(data_file, rec) == -1)
        {
            free_record(&rec);
            break;
        }

        // If the record is active and the name matches exactly, it exists
        if (rec->removed == FALSE &&
            rec->station_name != NULL &&
            strcmp(rec->station_name, name) == 0)
        {
            free_record(&rec);
            return 1;
        }
        free_record(&rec);
    }
    return 0; // Did not find the name
}

static int exists_station_pair(FILE *data_file, Header *header, int code, int next_code)
{
    // Start searching right after the header
    fseek(data_file, HEADER_SIZE, SEEK_SET);

    for (int i = 0; i < header->nextRRN; i++)
    {
        Record *rec = new_record();

        if (read_record(data_file, rec) == -1)
        {
            free_record(&rec);
            break;
        }

        // If the record is active and both the origin and destination codes match, the pair exists
        if (rec->removed == FALSE &&
            rec->station_code == code &&
            rec->next_station_code == next_code)
        {
            free_record(&rec);
            return 1;
        }
        free_record(&rec);
    }
    return 0; // Pair not found
}

// * Deletion operations

int delete_records()
{
    char data_filename[100];
    char index_filename[100];

    // Read filenames and number of deletion queries
    scanf("%s %s", data_filename, index_filename);
    int removals;
    scanf("%d", &removals);

    // Open files in read/write binary mode
    FILE *data_file = fopen(data_filename, "rb+");
    FILE *index_file = fopen(index_filename, "rb+");

    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    // Read and validate the binary header
    Header *header = read_binary_header(data_file);
    if (header == NULL || header->status != TRUE)
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(data_file);
        fclose(index_file);
        return FILE_NOT_FOUND;
    }

    // Set status to inconsistent during operations
    header->status = FALSE;
    save_header(data_file, header);

    int index_count;
    PrimaryIndex *indexes = load_indexes(index_file, &index_count);

    long data_offset = HEADER_SIZE;

    // Process each deletion query
    for (int op = 0; op < removals; op++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];
        read_criteria(criteria, num_fields);

        int count = 0;

        // Perform search to find all records matching the criteria
        Search_result *results = search_with_rrn(data_file, index_file, data_offset, criteria, num_fields, &count);

        if (count == 0)
        {
            free(results);
            continue;
        }

        // Iterate backwards through results to remove them safely
        for (int i = count - 1; i >= 0; i--)
        {
            int rrn = results[i].rrn;
            long offset = HEADER_SIZE + rrn * RECORD_SIZE;

            fflush(data_file);
            fseek(data_file, offset, SEEK_SET);

            char removed_flag;
            if (fread(&removed_flag, sizeof(char), 1, data_file) != 1)
                continue;

            // Skip if the record is already removed
            if (removed_flag == TRUE)
                continue;

            // Mark the record as removed and update the stack
            remove_record_by_rrn(data_file, header, rrn);

            // Check if removing this record eliminates a unique station name or pair
            if (results[i].record->station_name != NULL &&
                !exists_station_name(data_file, header, results[i].record->station_name))
                header->station_num--;

            if (results[i].record->next_station_code != -1 &&
                !exists_station_pair(data_file, header, results[i].record->station_code, results[i].record->next_station_code))
                header->station_pairs_num--;

            // Remove the reference from the index array in memory
            remove_index_entry(indexes, &index_count, results[i].record->station_code);

            free_record(&results[i].record);
        }

        free(results);
    }

    // Restore consistency and save changes
    header->status = TRUE;
    save_header(data_file, header);
    rewrite_index_file(index_filename, indexes, index_count);

    // Clean up
    fclose(data_file);
    fclose(index_file);
    free(indexes);
    free(header);

    // Hash validation
    BinarioNaTela(data_filename);
    BinarioNaTela(index_filename);

    return SUCCESS;
}

// * Insertion operations

void read_string_field(char **dest, int *size)
{
    char buffer[1000];
    scan_quote_string(buffer);

    // Handle missing strings explicitly
    if (strcmp(buffer, "NULO") == 0)
    {
        *dest = NULL;
        *size = 0;
        return;
    }

    // Allocate exact memory for the valid string
    *size = strlen(buffer);
    *dest = (char*)malloc(*size + 1);
    strcpy(*dest, buffer);
}

Record *read_insert_record()
{
    Record *rec = new_record();
    if (!rec)
        return NULL;

    char buffer[100];

    // Sequentially parse every field handling NULO conversions
    scanf("%s", buffer);
    rec->station_code = atoi(buffer);

    read_string_field(&rec->station_name, &rec->station_name_size);

    scanf("%s", buffer);
    rec->line_code = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    read_string_field(&rec->line_name, &rec->line_name_size);

    scanf("%s", buffer);
    rec->next_station_code = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    scanf("%s", buffer);
    rec->next_station_distance = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    scanf("%s", buffer);
    rec->line_integration_code = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    scanf("%s", buffer);
    rec->station_integration_code = (strcmp(buffer, "NULO") == 0) ? -1 : atoi(buffer);

    return rec;
}

int insert_records()
{
    char data_filename[100];
    char index_filename[100];

    scanf("%s %s", data_filename, index_filename);

    int insertions;
    scanf("%d", &insertions);

    FILE *data_file = fopen(data_filename, "rb+");
    FILE *index_file = fopen(index_filename, "rb+");

    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

    Header *header = read_binary_header(data_file);
    if (header == NULL || header->status != TRUE)
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(data_file);
        fclose(index_file);
        return FILE_NOT_FOUND;
    }

    header->status = FALSE;
    save_header(data_file, header);

    // Load existing index array and calculate capacity limits for potential resizing
    int index_count = 0;
    int capacity = 10;
    PrimaryIndex *indexes = load_indexes(index_file, &index_count);

    if (indexes != NULL)
    {
        capacity = index_count * 2;
        indexes = (PrimaryIndex*)realloc(indexes, capacity * sizeof(PrimaryIndex));
    }
    else
    {
        indexes = (PrimaryIndex*)malloc(capacity * sizeof(PrimaryIndex));
    }

    // Process each insertion request
    for (int op = 0; op < insertions; op++)
    {
        Record *rec = read_insert_record();
        if (!rec)
            continue;

        // Check uniqueness counters
        if (rec->station_name != NULL && !exists_station_name(data_file, header, rec->station_name))
        {
            header->station_num++;
        }

        if (rec->next_station_code != -1 && !exists_station_pair(data_file, header, rec->station_code, rec->next_station_code))
        {
            header->station_pairs_num++;
        }

        int rrn;

        // Manage RRN location: Reuse a logically removed space or append at the end
        if (header->top != -1)
        {
            rrn = header->top;

            // Fetch the next element in the removed stack to update header->top
            long offset = HEADER_SIZE + rrn * RECORD_SIZE;
            fseek(data_file, offset + 1, SEEK_SET);

            int next;
            fread(&next, sizeof(int), 1, data_file);
            header->top = next;
        }
        else
        {
            rrn = header->nextRRN;
            header->nextRRN++;
        }

        long offset = HEADER_SIZE + rrn * RECORD_SIZE;
        fseek(data_file, offset, SEEK_SET);

        rec->removed = FALSE;
        rec->next_record = -1;

        // Write the data to disk and update index dynamically
        save_record_to_bin(data_file, rec);
        insert_index_sorted(&indexes, &index_count, &capacity, rec->station_code, rrn);

        free_record(&rec);
    }

    // Confirm stability and overwrite files
    header->status = TRUE;
    save_header(data_file, header);
    rewrite_index_file(index_filename, indexes, index_count);

    fclose(data_file);
    fclose(index_file);
    free(indexes);
    free(header);

    BinarioNaTela(data_filename);
    BinarioNaTela(index_filename);

    return SUCCESS;
}

// * Update operations

void apply_updates(Record *rec, Search_criteria *updates, int p)
{
    // Iterate over each target field to apply its new value
    for (int i = 0; i < p; i++)
    {
        char *field = updates[i].field_name;
        char *value = updates[i].field_value;

        // Process fixed integer fields
        if (strcmp(field, "codEstacao") == 0)
        {
            if (strcmp(value, "NULO") != 0)
                rec->station_code = atoi(value); // Station code cannot realistically be NULL
        }
        else if (strcmp(field, "codLinha") == 0)
        {
            rec->line_code = (strcmp(value, "NULO") == 0) ? -1 : atoi(value);
        }
        else if (strcmp(field, "codProxEstacao") == 0)
        {
            rec->next_station_code = (strcmp(value, "NULO") == 0) ? -1 : atoi(value);
        }
        else if (strcmp(field, "distProxEstacao") == 0)
        {
            rec->next_station_distance = (strcmp(value, "NULO") == 0) ? -1 : atoi(value);
        }
        else if (strcmp(field, "codLinhaIntegra") == 0)
        {
            rec->line_integration_code = (strcmp(value, "NULO") == 0) ? -1 : atoi(value);
        }
        else if (strcmp(field, "codEstIntegra") == 0)
        {
            rec->station_integration_code = (strcmp(value, "NULO") == 0) ? -1 : atoi(value);
        }
        // Process dynamic string fields
        else if (strcmp(field, "nomeEstacao") == 0)
        {
            if (strcmp(value, "NULO") != 0)
            {
                if (rec->station_name != NULL)
                    free(rec->station_name);

                rec->station_name_size = strlen(value);
                rec->station_name = (char*)malloc(rec->station_name_size + 1);

                if (rec->station_name != NULL)
                    strcpy(rec->station_name, value);
            }
        }
        else if (strcmp(field, "nomeLinha") == 0)
        {
            if (rec->line_name != NULL)
                free(rec->line_name);

            if (strcmp(value, "NULO") == 0)
            {
                rec->line_name = NULL;
                rec->line_name_size = 0;
            }
            else
            {
                rec->line_name_size = strlen(value);
                rec->line_name = (char*)malloc(rec->line_name_size + 1);

                if (rec->line_name != NULL)
                    strcpy(rec->line_name, value);
            }
        }
    }
}

int update_records()
{
    char data_filename[100];
    char index_filename[100];

    scanf("%s", data_filename);
    scanf("%s", index_filename);

    int comparation_num;
    scanf("%d", &comparation_num);

    FILE *data_file = fopen(data_filename, READ_BINARY_MODE);
    FILE *index_file = fopen(index_filename, READ_BINARY_MODE);

    if (data_file == NULL || index_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

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

    // Set status to false immediately before writing updates
    temp_header->status = '0';
    save_header(data_file, temp_header);

    long data_offset = ftell(data_file);

    // Load entire index array to memory for fast lookups
    fseek(index_file, 0, SEEK_END);
    long index_size_bytes = ftell(index_file);

    int index_size = (index_size_bytes - 1) / sizeof(PrimaryIndex);
    PrimaryIndex *index_array = (PrimaryIndex*)malloc(index_size * sizeof(PrimaryIndex));

    fseek(index_file, 1, SEEK_SET);
    fread(index_array, sizeof(PrimaryIndex), index_size, index_file);

    // Perform operations for each update criteria
    for (int i = 0; i < comparation_num; i++)
    {
        int num_search_fields;
        scanf("%d", &num_search_fields);

        Search_criteria criteria_B[num_search_fields];
        read_criteria(criteria_B, num_search_fields);

        int num_update_fields;
        scanf("%d", &num_update_fields);

        Search_criteria criteria_A[num_update_fields];
        read_criteria(criteria_A, num_update_fields);

        int count = 0;

        // Perform the search to find target records
        Search_result *results = search_with_rrn(data_file, index_file, data_offset, criteria_B, num_search_fields, &count);

        if (count == 0)
        {
            continue;
        }

        // Apply changes to every matched record
        for (int j = 0; j < count; j++)
        {
            Record *rec = results[j].record;
            int rrn = results[j].rrn;
            int old_code = rec->station_code;

            // Modify in memory
            apply_updates(rec, criteria_A, num_update_fields);

            // Write modified record straight into its original offset
            long byte_offset = HEADER_SIZE + rrn * RECORD_SIZE;
            fseek(data_file, byte_offset, SEEK_SET);
            save_record_to_bin(data_file, rec);

            // Important: If the primary key changed, we must update the index array
            if (old_code != rec->station_code)
            {
                update_index_array(index_array, index_size, rrn, rec->station_code);
            }
            free_record(&rec);
        }
        free(results);
    }

    // Maintain index integrity by sorting it again
    qsort(index_array, index_size, sizeof(PrimaryIndex), compare_index);

    // Overwrite the index file completely
    rewind(index_file);
    char status = '1';
    fwrite(&status, sizeof(char), 1, index_file);
    fwrite(index_array, sizeof(PrimaryIndex), index_size, index_file);

    // Finalize header consistency and release locks
    temp_header->status = '1';
    save_header(data_file, temp_header);

    fclose(data_file);
    fclose(index_file);

    BinarioNaTela(data_filename);
    BinarioNaTela(index_filename);

    free(index_array);
    free(temp_header);

    return SUCCESS;
}