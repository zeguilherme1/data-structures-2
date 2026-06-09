#ifndef TOKENIZER_H
#define TOKENIZER_H
#define _POSIX_C_SOURCE 200809L
#include "../models/record.h"

/*
    tokenize_record: 
        This function receives a CSV line and converts it into a Record struct.

        It splits the buffer using commas, converts numeric fields,
        and allocates memory for string fields.

        Args:
            buffer: string containing a CSV line.

        Return:
            A pointer to the created Record or NULL in case of allocation error.

*/
Record *tokenize_record(
    char *buffer
);

/*
    meu_strtok: 
        This function splits a string using given delimiters.

        It works similarly to strtok, but keeps internal state through the buffer pointer.
        Each call returns the next token and updates the buffer.

        Args:
            buffer: pointer to the string pointer being processed.
            delimiter: characters used as delimiters.

        Return:
            Pointer to the next token or NULL if no tokens remain.

*/
char *meu_strtok(
    char **buffer,
    const char *delimiter
);

#endif