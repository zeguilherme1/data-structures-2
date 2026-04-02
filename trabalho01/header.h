#ifndef HEADER_H
#define HEADER_H
#define TRUE '1'
#define FALSE '0'


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct header {
	char status; 
    int top;
    int nextRRN;
    int station_num;
    int station_pairs_num;
} Header;

Header* new_header();

void save_header(FILE* bin_file, Header* bin_header);

int read_header(FILE* bin_file, Header* bin_header);

#endif
