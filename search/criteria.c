#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../constants.h"
#include "../models/header.h"
#include "../models/record.h"
#include "../models/index.h"
#include "../utils/input_utils.h"
#include "../io/binary_io.h"
#include "criteria.h"

void read_criteria(Search_criteria *criteria, int num_fields)
{
    for (int j = 0; j < num_fields; j++)
    {
        scanf("%s", criteria[j].field_name);
        // string fields need special reading (can contain spaces)
        if (strcmp(criteria[j].field_name, "nomeEstacao") == 0 ||
            strcmp(criteria[j].field_name, "nomeLinha") == 0)
        {
            scan_quote_string(criteria[j].field_value);
        }
        else
        {
            scanf("%s", criteria[j].field_value);
        }
    }
}

Record **sequential_search(FILE *fp, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    fseek(fp, data_offset, SEEK_SET); //start at first record

    Record **results = NULL;
    *count = 0;

    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(fp, temp_record);

        if (ret_record == -1) //EOF
        {
            free_record(&temp_record);
            return results;
        }
        //only consider valid records
        if (temp_record->removed == FALSE)
        {
            //check if all criterias matches
            if (matches_record_criteria(temp_record, criteria, num_fields) == 0)
            {
                //returns a array of found records
                results = realloc(results, (*count + 1) * sizeof(Record *));
                results[*count] = temp_record;
                (*count)++;
                continue;
            }
        }

        free_record(&temp_record);
    }
}

int get_station_code(Search_criteria *criteria, int num_fields)
{
    for (int i = 0; i < num_fields; i++)
    {
        //look for "codEstacao" in criteria
        if (strcmp(criteria[i].field_name, "codEstacao") == 0)
        {
            if (strcmp(criteria[i].field_value, "NULO") == 0)
                return NO_DATA_ERROR;

            return atoi(criteria[i].field_value); //convert to int
        }
    }
    return NO_DATA_ERROR;
}

int find_rrn_by_station_code(FILE *index_file, int has_station_code)
{
    fseek(index_file, 0, SEEK_END);
    long file_size = ftell(index_file);

    int total = (file_size - 1) / sizeof(PrimaryIndex);

    int left = 0;
    int right = total - 1;

    //uses binary search to find the correspondent rrn
    while (left <= right)
    {
        int mid = (left + right) / 2;

        fseek(index_file, 1 + mid * sizeof(PrimaryIndex), SEEK_SET);

        PrimaryIndex index;

        if (fread(&index, sizeof(PrimaryIndex), 1, index_file) != 1)
        {
            return NO_DATA_ERROR;
        }

        if (index.station_code == has_station_code)
        {
            return index.rrn; //found
        }
        else if (index.station_code < has_station_code)
        {
            left = mid + 1; //search in right half
        }
        else
        {
            right = mid - 1; //search in left half
        }
    }

    return NO_DATA_ERROR;
}

Record *indexed_search(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int has_station_code)
{
    if (has_station_code == NO_DATA_ERROR)
        return NULL;

    fseek(index_file, 0, SEEK_SET);
    int rrn = find_rrn_by_station_code(index_file, has_station_code);

    if (rrn == NO_DATA_ERROR)
        return NULL;

    fseek(data_file, data_offset, SEEK_SET);
    Record *result_record = read_rrn_record(data_file, rrn);

    if (result_record == NULL)
        return NULL;

    //validate record after fetching
    if (result_record->removed == FALSE &&
        matches_record_criteria(result_record, criteria, num_fields) == 0)
    {
        return result_record;
    }

    free_record(&result_record);
    return NULL;
}

Record **search_record(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    int has_station_code = get_station_code(criteria, num_fields);

    if (has_station_code != NO_DATA_ERROR && index_file != NULL)
    {
        Record *rec = indexed_search(data_file, index_file, data_offset, criteria, num_fields, has_station_code);

        if (rec == NULL)
        {
            *count = 0;
            return NULL;
        }

        Record **results = malloc(sizeof(Record *));
        results[0] = rec;
        *count = 1;

        return results;
    }

    return sequential_search(data_file, data_offset, criteria, num_fields, count);
}

int criteria_search()
{
    char bin_filename[100];
    scanf("%s", bin_filename);

    int comparation_num;
    scanf("%d", &comparation_num);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return FILE_NOT_FOUND;
    }

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

    long data_offset = ftell(bin_file); //start of records

    for (int i = 0; i < comparation_num; i++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];

        read_criteria(criteria, num_fields);

        int count = 0;
        Record **rec = sequential_search(bin_file, data_offset, criteria, num_fields, &count);

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

    long data_offset = ftell(data_file);

    for (int i = 0; i < comparation_num; i++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];

        read_criteria(criteria, num_fields);

        int count = 0;
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