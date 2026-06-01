#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../constants.h"
#include "../models/header.h"
#include "../models/record.h"
#include "../utils/input_utils.h"
#include "../io/binary_io.h"
#include "criteria.h"

int criteria_search()
{

    char bin_filename[100];
    scanf("%s", bin_filename);

    int comparaton_num;
    scanf("%d", &comparaton_num);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        return -1;
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
        return -1;
    }

    long data_offset = ftell(bin_file);

    for (int i = 0; i < comparaton_num; i++)
    {
        fseek(bin_file, data_offset, SEEK_SET);

        int num_fields;
        scanf("%d", &num_fields);

        Search_criteria criteria[num_fields];

        for (int j = 0; j < num_fields; j++)
        {
            scanf("%s", criteria[j].field_name);

            if (strcmp(criteria[j].field_name, "station_name") == 0 ||
                strcmp(criteria[j].field_name, "nomeLinha") == 0)
            {
                scan_quote_string(criteria[j].field_value);
            }
            else
            {
                scanf("%s", criteria[j].field_value);
            }
        }

        int found = 0;

        while (1)
        {
            Record *temp_record = new_record();
            int ret_record = read_record(bin_file, temp_record);

            if (ret_record == -1)
            {
                free_record(&temp_record);

                if (feof(bin_file))
                    break;
                else
                {
                    printf("Falha no processamento do arquivo.\n");
                    free(temp_header);
                    fclose(bin_file);
                    return -1;
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

        if (!found)
            printf("Registro inexistente.\n");

        if (i < comparaton_num - 1)
            printf("\n");
    }

    free(temp_header);
    fclose(bin_file);

    return 0;
}
