
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "../constants.h"
#include "../models/header.h"
#include "../models/record.h"
#include "../search/search.h"
#include "../models/index.h"
#include "../parser/tokenizer.h"
#include "../utils/utils.h"

#include "io.h"

// * Data conversion

int bin_to_text()
{
    char bin_filename[100];
    scanf("%s", bin_filename);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if (bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    Header *temp_header = new_header();
    if (temp_header == NULL)
        return MALLOC_ERROR;

    // Read the header to ensure the file is consistent and to skip the header bytes
    int ret_header = read_header(bin_file, temp_header);

    if (ret_header == -1 || temp_header->status != '1')
    {
        printf("Falha no processamento do arquivo.");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    // Sequentially read and print all records until EOF
    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(bin_file, temp_record);

        if (ret_record == -1) // Handling read failure or EOF
        {
            free_record(&temp_record);
            if (feof(bin_file))
                break; // End of file reached safely
            else
            {
                printf("Falha no processamento do arquivo.");
                free(temp_header);
                fclose(bin_file);
                return -1;
            }
        }
        else
        {
            // Only display logically active records
            if (temp_record->removed == FALSE)
                print_record(temp_record);

            free_record(&temp_record);
        }
    }

    free(temp_header);
    fclose(bin_file);

    return SUCCESS;
}

// * Header operations

Header *read_binary_header(FILE *bin_file)
{
    Header *bin_header = new_header();

    if (bin_header == NULL)
        return NULL;

    // Read sequentially all header fields from the binary file
    fread(&bin_header->status, sizeof(char), 1, bin_file);
    fread(&bin_header->top, sizeof(int), 1, bin_file);
    fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    fread(&bin_header->station_pairs_num, sizeof(int), 1, bin_file);

    return bin_header;
}

void save_header(FILE *bin_file, Header *header)
{
    // Go to the absolute start of the file before writing the header
    rewind(bin_file);

    // Write the fields strictly in the defined structural order
    fwrite(&header->status, sizeof(char), 1, bin_file);
    fwrite(&header->top, sizeof(int), 1, bin_file);
    fwrite(&header->nextRRN, sizeof(int), 1, bin_file);
    fwrite(&header->station_num, sizeof(int), 1, bin_file);
    fwrite(&header->station_pairs_num, sizeof(int), 1, bin_file);
}

int read_header(FILE *bin_file, Header *bin_header)
{
    if (bin_file == NULL || bin_header == NULL)
        return NO_DATA_ERROR;

    int verify = 0;

    // Verify tracks the success count of read operations
    verify += fread(&bin_header->status, sizeof(char), 1, bin_file);
    verify += fread(&bin_header->top, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_pairs_num, sizeof(int), 1, bin_file);

    // Ensure exactly 5 components were read
    if (verify == 5)
        return 0;
    else
        return -1;
}

// * Record operations

int read_record(FILE *bin_file, Record *bin_record)
{
    if (bin_file == NULL || bin_record == NULL)
        return NO_DATA_ERROR;

    // Store the file position pointer at the start of the record
    long start = ftell(bin_file);
    int verify = 0;

    // Read all the fixed-size base fields
    verify += fread(&bin_record->removed, sizeof(char), 1, bin_file);
    verify += fread(&bin_record->next_record, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->station_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->line_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->next_station_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->next_station_distance, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->line_integration_code, sizeof(int), 1, bin_file);
    verify += fread(&bin_record->station_integration_code, sizeof(int), 1, bin_file);

    // Read station name size and handle its dynamic string memory
    verify += fread(&bin_record->station_name_size, sizeof(int), 1, bin_file);
    if (bin_record->station_name_size > 0)
    {
        bin_record->station_name = (char*)malloc(bin_record->station_name_size + 1);
        if (bin_record->station_name == NULL)
            return NO_DATA_ERROR;

        verify += fread(bin_record->station_name, sizeof(char), bin_record->station_name_size, bin_file);
        bin_record->station_name[bin_record->station_name_size] = '\0';
    }
    else
    {
        bin_record->station_name = NULL;
    }

    // Read line name size and handle its dynamic string memory
    verify += fread(&bin_record->line_name_size, sizeof(int), 1, bin_file);
    if (bin_record->line_name_size > 0)
    {
        bin_record->line_name = (char*)malloc(bin_record->line_name_size + 1);
        if (bin_record->line_name == NULL)
            return NO_DATA_ERROR;

        verify += fread(bin_record->line_name, sizeof(char), bin_record->line_name_size, bin_file);
        bin_record->line_name[bin_record->line_name_size] = '\0';
    }
    else
    {
        bin_record->line_name = NULL;
    }

    // Calculate how many bytes have been read so far to map padding
    long end = ftell(bin_file);
    int bytes_read = end - start;

    // Calculate how many garbage/padding bytes exist until the end of the current record
    int remaining = RECORD_SIZE - bytes_read;

    // Skip the garbage filling by advancing the file pointer
    if (remaining > 0)
        fseek(bin_file, remaining, SEEK_CUR);

    // Validate if the core components of the record were successfully read
    if (verify >= 10)
        return 0;

    return -1;
}

void save_record(FILE *bin_filename, Record *new_record)
{
    // Write just the fixed-size structure data
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
    // Track where the write operation begins
    long start = ftell(bin_file);

    // Write all fixed-size base fields
    fwrite(&r->removed, sizeof(char), 1, bin_file);
    fwrite(&r->next_record, sizeof(int), 1, bin_file);
    fwrite(&r->station_code, sizeof(int), 1, bin_file);
    fwrite(&r->line_code, sizeof(int), 1, bin_file);
    fwrite(&r->next_station_code, sizeof(int), 1, bin_file);
    fwrite(&r->next_station_distance, sizeof(int), 1, bin_file);
    fwrite(&r->line_integration_code, sizeof(int), 1, bin_file);
    fwrite(&r->station_integration_code, sizeof(int), 1, bin_file);

    // Write station name size and string if applicable
    fwrite(&r->station_name_size, sizeof(int), 1, bin_file);
    if (r->station_name_size > 0)
        fwrite(r->station_name, 1, r->station_name_size, bin_file);

    // Write line name size and string if applicable
    fwrite(&r->line_name_size, sizeof(int), 1, bin_file);
    if (r->line_name_size > 0)
        fwrite(r->line_name, 1, r->line_name_size, bin_file);

    // Calculate how many bytes were actually written to the file
    long end = ftell(bin_file);
    int written = end - start;

    // Safety abort in case of a buffer overflow mapping structure
    if (written > RECORD_SIZE)
    {
        printf("ERRO GRAVE: registro maior que RECORD_SIZE\n");
        exit(1);
    }

    // Determine how many padding characters are needed to match the fixed size
    int remaining = RECORD_SIZE - written;

    // Write the padding character '$' to fill the rest of the record space
    char trash = '$';
    for (int i = 0; i < remaining; i++)
        fwrite(&trash, 1, 1, bin_file);
}


