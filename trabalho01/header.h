#ifndef HEADER_H
#define HEADER_H
#define TRUE '1'
#define FALSE '0'

typedef struct header Header;

Header* create_header();

void save_header(FILE* bin_file, Header* bin_header);

#endif
