#ifndef INDEX_H
#define INDEX_H

typedef struct {
    int station_code;
    int rrn;
} PrimaryIndex;

void create_index_file();

#endif