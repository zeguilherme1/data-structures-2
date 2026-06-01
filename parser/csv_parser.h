#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#define _POSIX_C_SOURCE 200809L
#include "../models/record.h"

Record *tokenize_record(
    char *buffer
);

#endif