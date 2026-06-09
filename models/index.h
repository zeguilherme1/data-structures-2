#ifndef INDEX_H
#define INDEX_H

typedef struct {
    int station_code;
    int rrn;
} PrimaryIndex;

void create_index_file();
int update_index_array(PrimaryIndex *index_array, int size, int rrn, int new_code);
int compare_index(const void *a, const void *b);

#endif