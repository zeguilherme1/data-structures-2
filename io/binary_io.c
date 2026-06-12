#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"
#include "../search/criteria.h"
#include "../models/index.h"
#include "../search/rrn.h"
#include "../utils/debug_utils.h"

#include "binary_io.h"

Header *read_binary_header(FILE *bin_file)
{
    Header *bin_header = new_header();

    if (bin_header == NULL)
        return NULL;

    fread(&bin_header->status, sizeof(char), 1, bin_file);
    fread(&bin_header->top, sizeof(int), 1, bin_file);
    fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    fread(&bin_header->station_pairs_num, sizeof(int), 1, bin_file);

    return bin_header;
}

void save_header(FILE *bin_file, Header *header)
{
    rewind(bin_file);
    fwrite(&header->status, sizeof(char), 1, bin_file);
    fwrite(&header->top, sizeof(int), 1, bin_file);
    fwrite(&header->nextRRN, sizeof(int), 1, bin_file);
    fwrite(&header->station_num, sizeof(int), 1, bin_file);
    fwrite(&header->station_pairs_num, sizeof(int), 1, bin_file);
}

int read_header(FILE *bin_file, Header *bin_header)
{
    if (bin_file == NULL)
        return NO_DATA_ERROR;
    if (bin_header == NULL)
        return NO_DATA_ERROR;

    int verify = 0;

    verify += fread(&bin_header->status, sizeof(char), 1, bin_file);
    verify += fread(&bin_header->top, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_pairs_num, sizeof(int), 1, bin_file);

    if (verify == 5)
        return 0;
    else
        return -1;
}

int read_record(FILE *bin_file, Record *bin_record)
{

    if (bin_file == NULL || bin_record == NULL)
        return NO_DATA_ERROR;

    long start = ftell(bin_file);
    int verify = 0;


    verify += fread(&bin_record->removed, sizeof(char), 1, bin_file);
    verify += fread(&bin_record->next_record, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->station_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->line_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->next_station_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->next_station_distance, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->line_integration_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->station_integration_code, sizeof(int), 1, bin_file);

    verify += fread(&bin_record->station_name_size, sizeof(int), 1, bin_file);
    if (bin_record->station_name_size > 0)
    {
        bin_record->station_name = malloc(bin_record->station_name_size + 1);
        if (bin_record->station_name == NULL)
            return NO_DATA_ERROR;

        verify += fread(bin_record->station_name, sizeof(char), bin_record->station_name_size, bin_file);

        bin_record->station_name[bin_record->station_name_size] = '\0';
    }
    else
    {
        bin_record->station_name = NULL;
    }

    verify += fread(&bin_record->line_name_size, sizeof(int), 1, bin_file);
    if (bin_record->line_name_size > 0)
    {
        bin_record->line_name = malloc(bin_record->line_name_size + 1);
        if (bin_record->line_name == NULL)
            return NO_DATA_ERROR;
        verify += fread(bin_record->line_name, sizeof(char), bin_record->line_name_size, bin_file);

        bin_record->line_name[bin_record->line_name_size] = '\0';
    }
    else
    {
        bin_record->line_name = NULL;
    }

    char trash[43];
    int trash_size = 43 - bin_record->station_name_size - bin_record->line_name_size;

    long end = ftell(bin_file);
    int bytes_read = end - start;

    int remaining = RECORD_SIZE - bytes_read;

    if (remaining > 0)
        fseek(bin_file, remaining, SEEK_CUR);

    // valida leitura
    if (verify >= 10) // mínimo esperado
        return 0;

    return -1;
}

void save_record(FILE *bin_filename, Record *new_record)
{
    fwrite(&new_record->removed, sizeof(char), 1, bin_filename);
    fwrite(&new_record->next_record, sizeof(int), 1, bin_filename);
    fwrite(&new_record->station_code, sizeof(int), 1, bin_filename);
    fwrite(&new_record->line_code, sizeof(int), 1, bin_filename);
    fwrite(&new_record->next_station_code, sizeof(int), 1, bin_filename);
    fwrite(&new_record->next_station_distance, sizeof(int), 1, bin_filename);
    fwrite(&new_record->line_integration_code, sizeof(int), 1, bin_filename);
    fwrite(&new_record->station_integration_code, sizeof(int), 1, bin_filename);
}

void save_record_to_bin(FILE *bin_file, Record *r)
{
    long start = ftell(bin_file);

    fwrite(&r->removed, sizeof(char), 1, bin_file);
    fwrite(&r->next_record, sizeof(int), 1, bin_file);
    fwrite(&r->station_code, sizeof(int), 1, bin_file);
    fwrite(&r->line_code, sizeof(int), 1, bin_file);
    fwrite(&r->next_station_code, sizeof(int), 1, bin_file);
    fwrite(&r->next_station_distance, sizeof(int), 1, bin_file);
    fwrite(&r->line_integration_code, sizeof(int), 1, bin_file);
    fwrite(&r->station_integration_code, sizeof(int), 1, bin_file);

    fwrite(&r->station_name_size, sizeof(int), 1, bin_file);
    if (r->station_name_size > 0)
        fwrite(r->station_name, 1, r->station_name_size, bin_file);

    fwrite(&r->line_name_size, sizeof(int), 1, bin_file);
    if (r->line_name_size > 0)
        fwrite(r->line_name, 1, r->line_name_size, bin_file);

    long end = ftell(bin_file);
    int written = end - start;

    if (written > RECORD_SIZE)
    {
        printf("ERRO GRAVE: registro maior que RECORD_SIZE\n");
        exit(1);
    }

    int remaining = RECORD_SIZE - written;

    char trash = '$';
    for (int i = 0; i < remaining; i++)
        fwrite(&trash, 1, 1, bin_file);
}

Record *read_rrn_record(FILE *bin_file, int rrn)
{
    int byte_offset = (HEADER_SIZE + (RECORD_SIZE * rrn));
    fseek(bin_file, byte_offset, SEEK_SET);

    Record *find_record = (Record *)malloc(sizeof(Record));
    if (find_record == NULL)
        return NULL;

    char removido;
    fread(&removido, sizeof(char), 1, bin_file);

    find_record->removed = removido;

    if (removido == TRUE)
    {
        free(find_record);
        return NULL;
    }

    fread(&find_record->next_record, sizeof(int), 1, bin_file);
    fread(&find_record->station_code, sizeof(int), 1, bin_file);
    fread(&find_record->line_code, sizeof(int), 1, bin_file);
    fread(&find_record->next_station_code, sizeof(int), 1, bin_file);
    fread(&find_record->next_station_distance, sizeof(int), 1, bin_file);
    fread(&find_record->line_integration_code, sizeof(int), 1, bin_file);
    fread(&find_record->station_integration_code, sizeof(int), 1, bin_file);

    fread(&find_record->station_name_size, sizeof(int), 1, bin_file);
    if (find_record->station_name_size > 0)
    {
        find_record->station_name = malloc(find_record->station_name_size + 1);
        fread(find_record->station_name, sizeof(char), find_record->station_name_size, bin_file);
        find_record->station_name[find_record->station_name_size] = '\0';
    }
    else
    {
        find_record->station_name = NULL;
    }

    fread(&find_record->line_name_size, sizeof(int), 1, bin_file);
    if (find_record->line_name_size > 0)
    {
        find_record->line_name = malloc(find_record->line_name_size + 1);
        fread(find_record->line_name, sizeof(char), find_record->line_name_size, bin_file);
        find_record->line_name[find_record->line_name_size] = '\0';
    }
    else
    {
        find_record->line_name = NULL;
    }

    return find_record;
}


void apply_updates(Record *rec, Search_criteria *updates, int p)
{
    for(int i = 0; i < p; i++)
    {
        char *field = updates[i].field_name;
        char *value = updates[i].field_value;
        
        //for integer fields
        if (strcmp(field, "codEstacao") == 0)
        {
            if (strcmp(value, "NULO") != 0)
                rec->station_code = atoi(value); //station code can't be NULL
        }
        //if the value is "NULO" store -1 
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
        //for string fields
        else if (strcmp(field, "nomeEstacao") == 0)
        {
            if (strcmp(value, "NULO") != 0)
            {
                if (rec->station_name != NULL)
                    free(rec->station_name);
                rec->station_name_size = strlen(value);
                rec->station_name = malloc(rec->station_name_size + 1);
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
                rec->line_name = malloc(rec->line_name_size + 1);
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

    temp_header->status = '0';
    save_header(data_file, temp_header);

    long data_offset = ftell(data_file);
    
    //store index in memory - CONFERIR SE PODE SER ASSIM!!
    fseek(index_file, 0, SEEK_END);
    long index_size_bytes = ftell(index_file);

    int index_size = (index_size_bytes - 1) / sizeof(PrimaryIndex);

    PrimaryIndex *index_array = malloc(index_size * sizeof(PrimaryIndex));

    fseek(index_file, 1, SEEK_SET);
    fread(index_array, sizeof(PrimaryIndex), index_size, index_file);
    
    //do n updates
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

        Search_result *results = search_with_rrn(data_file, index_file, data_offset, criteria_B, num_search_fields, &count);
        
        if(count == 0){
            continue;
        }

        for(int j = 0; j < count; j++)
        {
            Record *rec = results[j].record;
            int rrn = results[j].rrn;
            int old_code = rec->station_code;

            apply_updates(rec, criteria_A, num_update_fields);
            
            long byte_offset = HEADER_SIZE + rrn * RECORD_SIZE;
            fseek(data_file, byte_offset, SEEK_SET);
            save_record_to_bin(data_file, rec);

            if(old_code != rec -> station_code)
            {
                update_index_array(index_array, index_size, rrn, rec->station_code);
            }
            free_record(&rec);
        }
        free(results);
    }
    //rewrites the index
    qsort(index_array, index_size, sizeof(PrimaryIndex), compare_index);

    rewind(index_file);
    char status = '1';
    fwrite(&status, sizeof(char), 1, index_file);
    fwrite(index_array, sizeof(PrimaryIndex), index_size, index_file);

    //update header status, close files and print binary
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

int delete_records()
{
    char data_filename[100];
    char index_filename[100];

    scanf("%s %s",
          data_filename,
          index_filename);

    int removals;
    scanf("%d", &removals);

    FILE *data_file =
        fopen(data_filename, "rb+");

    FILE *index_file =
        fopen(index_filename, "rb+");

    if(data_file == NULL ||
       index_file == NULL)
    {
        printf(
            "Falha no processamento do arquivo.\n"
        );
        return FILE_NOT_FOUND;
    }

    Header *header =
        read_binary_header(data_file);

    if(header == NULL ||
       header->status != TRUE)
    {
        printf(
            "Falha no processamento do arquivo.\n"
        );

        fclose(data_file);
        fclose(index_file);

        return FILE_NOT_FOUND;
    }

    header->status = FALSE;
    save_header(data_file, header);

    int index_count;

    PrimaryIndex *indexes =
        load_indexes(
            index_file,
            &index_count
        );

    long data_offset = HEADER_SIZE;

    for(int op = 0; op < removals; op++)
    {
        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];

        read_criteria(
            criteria,
            num_fields
        );

        int count = 0;

        Search_result *results =
            search_with_rrn(
                data_file,
                index_file,
                data_offset,
                criteria,
                num_fields,
                &count
            );

        if(results == NULL)
            continue;

        for(int i = 0; i < count; i++)
        {
            remove_record_by_rrn(
                data_file,
                header,
                results[i].rrn
            );

            remove_index_entry(
                indexes,
                &index_count,
                results[i].record->station_code
            );

            free_record(
                &results[i].record
            );
        }

        free(results);
    }

    header->status = TRUE;

    save_header(
        data_file,
        header
    );

    rewrite_index_file(
        index_filename,
        indexes,
        index_count
    );

    fclose(data_file);
    fclose(index_file);

    free(indexes);
    free(header);

    BinarioNaTela(data_filename);
    BinarioNaTela(index_filename);

    return SUCCESS;
}