#include <stdio.h>
#include <stdlib.h>

#include "../constants.h"

#include "../models/header.h"
#include "../models/record.h"
#include "../io/binary_io.h"

#include "rrn.h"
#include "criteria.h"

Search_result *search_with_rrn(FILE *data_file, FILE *index_file, long data_offset, Search_criteria *criteria, int num_fields, int *count)
{
    *count = 0;
    int capacity = 10; //defines a capacity and if it's ultrapassed realloc more memory
    Search_result *results = malloc(capacity * sizeof(Search_result));

    int has_station_code = get_station_code(criteria, num_fields);
    
    //index search (if has codEstacao)
    if(has_station_code != NO_DATA_ERROR && index_file != NULL)
    {
        fseek(index_file, 0, SEEK_SET);
        int rrn = find_rrn_by_station_code(index_file, has_station_code);
        
        if(rrn == NO_DATA_ERROR) return NULL;

        Record *rec = read_rrn_record(data_file, rrn);
        
        //check if all criteria matches
        if(rec != NULL && 
            rec->removed == FALSE &&
            matches_record_criteria(rec, criteria, num_fields) == 0)
        {
            //retuns a array with the find record 
            results = malloc(sizeof(Search_result));
            results[0].record = rec;
            results[0].rrn = rrn;
            *count = 1;
            return results;
        }
        if(rec != NULL) free_record(&rec);
        return NULL;
    }
    //sequential search
    fseek(data_file, data_offset, SEEK_SET);

    int rrn = 0;
    
    while(1)
    {
        Record *rec = new_record();
        int ret = read_record(data_file, rec);

        if(ret == -1)
        {
            free_record(&rec);
            break;
        }
        //check if all criteria matches
        if(rec->removed == FALSE &&
            matches_record_criteria(rec, criteria, num_fields) == 0)
        {
            //reallocate if capacity is exceeded
            if (*count == capacity)
            {
                capacity = (capacity == 0) ? 10 : capacity * 2;

                Search_result *temp = realloc(results, capacity * sizeof(Search_result));
                if (temp == NULL)
                {
                    for (int i = 0; i < *count; i++)
                        free_record(&results[i].record);

                    free(results);
                    free_record(&rec);
                    return NULL;
                }

                results = temp;
            }
            //store the found results in search_results struct
            results[*count].record = rec;
            results[*count].rrn = rrn;
            (*count)++;
            rrn++;
            continue;
        }

        free_record(&rec);
        rrn++;
    }
    return results;
}

int search_rrn()
{
    char bin_filename[100];
    int rrn;

    scanf("%s %d", bin_filename, &rrn); //read name file and rrn

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if (bin_file == NULL){
        printf("Falha no processamento do arquivo.");
        return -1;
    }

    Header *bin_header = read_binary_header(bin_file);

    if (bin_header == NULL)
        return MALLOC_ERROR;

    //validate rrn bounds
    if (rrn < 0 || rrn >= bin_header->nextRRN)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }
    //direct acess to record using rrn
    Record *result_record = read_rrn_record(bin_file, rrn);
    if (result_record == NULL)
    {
        printf("Registro inexistente.\n");
        fclose(bin_file);
        return NO_DATA_ERROR;
    }

    print_record(result_record); //print found record
    free(bin_header);
}