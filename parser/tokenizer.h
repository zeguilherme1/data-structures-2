#ifndef TOKENIZER_H
#define TOKENIZER_H
#define _POSIX_C_SOURCE 200809L
#include "../models/record.h"

Record *tokenize_record(
    char *buffer
);

char *meu_strtok(
    char **buffer,
    const char *delimiter
);

#endif