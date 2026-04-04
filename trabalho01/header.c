#include <stdio.h>
#include "header.h"
#include "utils.h"

Header* new_header() {
    /*
        This function creates a new header and set up initial values

        Args:
            No args

        Return:
            new_header: The generated header
    
    */

    Header* new_header = (Header*)malloc(sizeof(Header));

    if(new_header == NULL) {
        return NULL;
    }

    new_header->status = TRUE;
    new_header->top = -1;
    new_header->nextRRN = 0;
    new_header->station_num = 0;
    new_header->station_pairs_num = 0;

    return new_header;
}

void save_header(FILE* bin_file, Header* header) {
    /*
        This function save a header struct to a binary file

        Args:
            (FILE*) bin_file: binary output file
            (Header*) header: the header struct that will be saved

    */

   rewind(bin_file);

   fwrite(&header->status, sizeof(char), 1, bin_file);
   fwrite(&header->top, sizeof(int), 1, bin_file);
   fwrite(&header->nextRRN, sizeof(int), 1, bin_file);
   fwrite(&header->station_num, sizeof(int), 1, bin_file);
   fwrite(&header->station_pairs_num, sizeof(int), 1, bin_file);
}

Header* read_binary_header(FILE* bin_file) {
    Header* bin_header = new_header();

    if(bin_header == NULL) return NULL;

    fread(&bin_header->status, sizeof(char), 1, bin_file);
    fread(&bin_header->top, sizeof(int), 1, bin_file);
    fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    fread(&bin_header->station_pairs_num, sizeof(int), 1 , bin_file);

    return bin_header;
}

int read_header(FILE* bin_file, Header* bin_header){
    /*
        This function confirm the if header was successful read

        Args:
            (FILE*) bin_file: binary input file
            (Header*) header: the header struct that will be read

        Return:
            0 for success and -1 for fail
    
    */

    if(bin_file == NULL) return NO_DATA_ERROR;
    if(bin_header == NULL) return NO_DATA_ERROR;

    int verify = 0;

    verify += fread(&bin_header->status, sizeof(char), 1, bin_file);
    verify += fread(&bin_header->top, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->nextRRN, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_num, sizeof(int), 1, bin_file);
    verify += fread(&bin_header->station_pairs_num, sizeof(int), 1, bin_file);

    if(verify == 5) return 0;
    else return -1;
}


