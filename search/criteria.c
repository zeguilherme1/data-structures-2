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

void read_criteria(Search_criteria *criteria, int num_fields){
    for (int j = 0; j < num_fields; j++)
    {
        scanf("%s", criteria[j].field_name);

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

int sequential_scan_with_criteria(FILE *fp, long data_offset, Search_criteria *criteria, int num_fields){
    fseek(fp, data_offset, SEEK_SET);

    int found = 0;

    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(fp, temp_record);

        if (ret_record == -1)
        {
            free_record(&temp_record);

            if (feof(fp))
                break;
            else
            {
                return FILE_NOT_FOUND;
            }
        }

        if (temp_record->removed == FALSE)
        {
            if (matches_record_criteria(temp_record, criteria, num_fields) == 0)
            {
                print_record(temp_record);
                found = 1;
            }
        }

        free_record(&temp_record);
    }

    return found;
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

    long data_offset = ftell(bin_file);

    for (int i = 0; i < comparation_num; i++)
    {
        fseek(bin_file, data_offset, SEEK_SET);

        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];

        read_criteria(criteria, num_fields);
    
        int search_result = sequential_scan_with_criteria(bin_file, data_offset, criteria, num_fields);

        if(search_result == FILE_NOT_FOUND){
            printf("Falha no processamento do arquivo!\n");
            free(temp_header);
            fclose(bin_file);
            return FILE_NOT_FOUND;
        }
        if (!search_result)
            printf("Registro inexistente.\n");

        if (i < comparation_num - 1)
            printf("\n");
    }

    free(temp_header);
    fclose(bin_file);

    return 0;
}

int get_station_code(Search_criteria *criteria, int num_fields)
{
    for (int i = 0; i < num_fields; i++)
    {
        if (strcmp(criteria[i].field_name, "codEstacao") == 0)
        {
            if (strcmp(criteria[i].field_value, "NULO") == 0)
                return NO_DATA_ERROR;

            return atoi(criteria[i].field_value);
        }
    }
    return NO_DATA_ERROR;
}

int find_rrn_by_station_code(FILE *index_file, int has_station_code){
    fseek(index_file, 0, SEEK_END);
    long file_size = ftell(index_file);

    int total = file_size / sizeof(PrimaryIndex);

    int left = 0;
    int right = total - 1;

    while(left <= right){
        int mid = (left + right) / 2;

        fseek(index_file, mid * sizeof(PrimaryIndex), SEEK_SET);

        PrimaryIndex index;
        fread(&index, sizeof(PrimaryIndex), 1, index_file);

        if(index.station_code == has_station_code){
            return index.rrn;
        } else if (index.station_code < has_station_code){
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return NO_DATA_ERROR;
}

int index_or_criteria_search(){
    char data_filename[100];
    char index_filename[100];

    scanf("%s", data_filename);
    scanf("%s", index_filename);

    int comparation_num;
    scanf("%d", &comparation_num);

    FILE *data_file = fopen(data_filename, READ_BINARY_MODE);
    FILE *index_file = fopen(index_filename, READ_BINARY_MODE);

    if(data_file == NULL || index_file == NULL){
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

        int has_station_code = get_station_code(criteria, num_fields);

        int found = 0;

        if(has_station_code != NO_DATA_ERROR){
            fseek(index_file, 0, SEEK_SET);
            int rrn = find_rrn_by_station_code(index_file, has_station_code);
            
            if(rrn != NO_DATA_ERROR){
                fseek(data_file, data_offset, SEEK_SET);
                
                Record *result_record = read_rrn_record(data_file, rrn);

                if (result_record == NULL)
                {
                    printf("Registro inexistente.\n");
                    free_record(&result_record);
                    fclose(index_file);
                    fclose(data_file);
                    free(temp_header);
                    return NO_DATA_ERROR;
                }
                if(result_record->removed == FALSE &&
                matches_record_criteria(result_record, criteria, num_fields) == 0)
                {
                    print_record(result_record);
                    found = 1;
                }
                free_record(&result_record);
            }

        } else{
            fseek(data_file, data_offset, SEEK_SET);

            int search_result = sequential_scan_with_criteria(data_file, data_offset, criteria, num_fields);

            if(search_result == FILE_NOT_FOUND){
                printf("Falha no processamento do arquivo.\n");
                fclose(data_file);
                fclose(index_file);
                free(temp_header);
                return FILE_NOT_FOUND;
            }

            found = search_result;
        }

        if(!found){
            printf("Registro inexistente.\n");
        }
        if(i < comparation_num - 1){
            printf("\n");
        }
    }
    fclose(data_file);
    fclose(index_file);
    free(temp_header);
    return SUCCESS;
}
