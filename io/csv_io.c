#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"

#include "../parser/tokenizer.h"
#include "../utils/string_utils.h"
#include "../utils/debug_utils.h"

#include "binary_io.h"
#include "csv_io.h"

int csv_to_bin()
{

    char csv_filename[100], bin_filename[100];

    scanf("%s %s", csv_filename, bin_filename);

    FILE *csv_file = fopen(csv_filename, "r");
    FILE *bin_file = fopen(bin_filename, WRITE_BINARY_MODE);

    if (csv_file == NULL || bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return 0;
    }

    Header *temp_header = new_header();
    if (!temp_header)
        return MALLOC_ERROR;

    temp_header->status = FALSE;
    save_header(bin_file, temp_header);

    char buffer[200];
    fgets(buffer, sizeof(buffer), csv_file);
    int record_counter = 0;

    Pair *pairs = calloc(10000, sizeof(Pair));
    int pair_counter = 0;

    char stations[10000][100];
    int station_counter = 0;

    while (fgets(buffer, sizeof(buffer), csv_file))
    {
        Record *new_record = tokenize_record(buffer);
        if (!new_record)
            continue;

        clean_string(new_record->station_name);

        new_record->removed = FALSE;
        new_record->next_record = temp_header->top;

        if (new_record->next_station_code != -1)
        {
            bool exists = false;

            for (int i = 0; i < pair_counter; i++)
            {
                if (pairs[i].station_code == new_record->station_code &&
                    pairs[i].next_station_code == new_record->next_station_code)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                pairs[pair_counter].station_code = new_record->station_code;
                pairs[pair_counter].next_station_code = new_record->next_station_code;
                pair_counter++;
            }
        }

        int station_exists = 0;

        for (int i = 0; i < station_counter; i++)
        {
            if (strcmp(stations[i], new_record->station_name) == 0)
            {
                station_exists = 1;
                break;
            }
        }

        if (!station_exists)
        {
            strcpy(stations[station_counter], new_record->station_name);
            station_counter++;
        }

        save_record_to_bin(bin_file, new_record);
        record_counter++;

        free_record(&new_record);
    }

    temp_header->status = TRUE;
    temp_header->nextRRN = record_counter;
    temp_header->station_num = station_counter;
    temp_header->station_pairs_num = pair_counter;

    save_header(bin_file, temp_header);

    fclose(csv_file);
    fclose(bin_file);
    free(temp_header);
    BinarioNaTela(bin_filename);

    return SUCCESS;
}

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

    int ret_header = read_header(bin_file, temp_header); // reads the header to skip its bytes

    if (ret_header == -1 || temp_header->status != '1')
    {
        printf("Falha no processamento do arquivo.");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    while (1)
    {
        Record *temp_record = new_record();
        int ret_record = read_record(bin_file, temp_record); // sequentially reads the record
        if (ret_record == -1)
        {
            free_record(&temp_record);
            if (feof(bin_file))
                break; // checks if the end of file was reached
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
            if (temp_record->removed == FALSE)
                print_record(temp_record); // if it was not removed, prints it
            free_record(&temp_record);
        }
    }

    free(temp_header);
    fclose(bin_file);

    return SUCCESS;
}