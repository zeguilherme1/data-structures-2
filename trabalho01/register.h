#ifndef REGISTER_H
#define REGISTER_H

#define HEADER_SIZE 16
#define REGISTER_SIZE 80
#define READ_BINARY_MODE "rb+"

// we are using record instead of "register"
// because register is a reserved keyword in C

typedef struct record Record;

int write_register(char *filename, Record* new_record);

Record* read_register_RRN(char* filename, int RRN);

#endif
