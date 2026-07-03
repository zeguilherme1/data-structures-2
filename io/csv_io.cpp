
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"

#include "../parser/tokenizer.h"
#include "../utils/utils.h"

#include "io.h"

// * Data conversion

int csv_to_bin()
{
    char csv_filename[100], bin_filename[100];

    // Read the input and output filenames
    scanf("%s %s", csv_filename, bin_filename);

    // Open CSV for reading and binary file for writing
    FILE *csv_file = fopen(csv_filename, "r");
    FILE *bin_file = fopen(bin_filename, WRITE_BINARY_MODE);

    // Validate file pointers
    if (csv_file == NULL || bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    // Allocate and initialize a new header
    Header *temp_header = new_header();
    if (!temp_header)
        return MALLOC_ERROR;

    // Set file status to FALSE (inconsistent) while writing
    temp_header->status = FALSE;
    save_header(bin_file, temp_header);

    char buffer[200];

    // Read and discard the first line of the CSV (column headers)
    fgets(buffer, sizeof(buffer), csv_file);

    int record_counter = 0;

    // Trackers to calculate unique stations and station pairs
    Pair *pairs = (Pair*)calloc(10000, sizeof(Pair));
    int pair_counter = 0;

    char stations[10000][100];
    int station_counter = 0;

    // Loop through each remaining line in the CSV file
    while (fgets(buffer, sizeof(buffer), csv_file))
    {
        // Parse the CSV line into a structured Record
        Record *new_record = tokenize_record(buffer);
        if (!new_record)
            continue;

        // Clean up string artifacts
        clean_string(new_record->station_name);

        // Initialize internal management fields
        new_record->removed = FALSE;
        new_record->next_record = temp_header->top;

        // Logic to track unique connected station pairs
        if (new_record->next_station_code != -1)
        {
            bool exists = false;

            // Check if the current pair already exists in our tracked array
            for (int i = 0; i < pair_counter; i++)
            {
                if (pairs[i].station_code == new_record->station_code &&
                    pairs[i].next_station_code == new_record->next_station_code)
                {
                    exists = true;
                    break;
                }
            }

            // If it's a new pair, register it
            if (!exists)
            {
                pairs[pair_counter].station_code = new_record->station_code;
                pairs[pair_counter].next_station_code = new_record->next_station_code;
                pair_counter++;
            }
        }

        int station_exists = 0;

        // Logic to track unique station names
        for (int i = 0; i < station_counter; i++)
        {
            if (strcmp(stations[i], new_record->station_name) == 0)
            {
                station_exists = 1;
                break;
            }
        }

        // If the station name is new, register it
        if (!station_exists)
        {
            strcpy(stations[station_counter], new_record->station_name);
            station_counter++;
        }

        // Write the parsed and processed record to the binary file
        save_record_to_bin(bin_file, new_record);
        record_counter++;

        // Free the dynamically allocated record from memory
        free_record(&new_record);
    }

    // Update the header with the final statistics
    temp_header->status = TRUE; // Mark as consistent
    temp_header->nextRRN = record_counter;
    temp_header->station_num = station_counter;
    temp_header->station_pairs_num = pair_counter;

    // Overwrite the initial header with the updated one
    save_header(bin_file, temp_header);

    // Clean up resources and close files
    fclose(csv_file);
    fclose(bin_file);
    free(pairs);
    free(temp_header);

    // Output the hash validation for the runcodes platform
    BinarioNaTela(bin_filename);

    return SUCCESS;
}