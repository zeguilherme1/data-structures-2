#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"

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

    if (trash_size > 0)
        fread(trash, sizeof(char), trash_size, bin_file);

    if (verify == 10 + bin_record->station_name_size + bin_record->line_name_size)
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
        find_record->station_name = (char *)calloc(find_record->station_name_size, sizeof(char));
    }
    fread(find_record->station_name, sizeof(char), find_record->station_name_size, bin_file);

    fread(&find_record->line_name_size, sizeof(int), 1, bin_file);
    if (find_record->line_name_size > 0)
    {
        find_record->line_name = (char *)calloc(find_record->line_name_size, sizeof(char));
    }
    fread(find_record->line_name, sizeof(char), find_record->line_name_size, bin_file);

    return find_record;
}