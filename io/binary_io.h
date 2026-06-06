#ifndef BINARY_IO_H
#define BINARY_IO_H


#include <stdio.h>
#include "../models/header.h"
#include "../models/record.h"

#define HEADER_SIZE 17
#define RECORD_SIZE 80

#define READ_BINARY_MODE "rb"
#define WRITE_BINARY_MODE "wb+"

Header *read_binary_header(FILE *bin_file);

void save_header(
    FILE *bin_file,
    Header *header
);

int read_header(
    FILE *bin_file,
    Header *header
);

int read_record(
    FILE *bin_file,
    Record *record
);

void save_record(
    FILE *bin_file,
    Record *record
);

void save_record_to_bin(
    FILE *bin_file,
    Record *record
);

Record *read_rrn_record(
    FILE *bin_file,
    int rrn
);

int search_rrn();
#endif