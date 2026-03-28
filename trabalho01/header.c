#include <stdio.h>
#include "header.h"

struct header {
	char status; 
    int top;
    int nextRRN;
    int station_num;
    int station_pairs_num;
};

Header* new_header() {
    /*
        This function creates a new header and set up initial values

        Args:
            No args

        Return:
            new_header: The generated header
    
    */

    Header* new_header = (Header*)malloc(sizeof(Header));

    if(!new_header) {
        return NULL;
    }

    new_header->status = TRUE;
    new_header->top = -1;
    new_header->nextRRN = 0;
    new_header->station_num = 0;
    new_header->station_pairs_num = 0;

    return new_header;
}

void save_reader(FILE* bin_file, Header* header) {
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


