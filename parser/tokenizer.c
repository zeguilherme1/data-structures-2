#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *meu_strtok(char **buffer, const char *delimitador)
{
    if (buffer == NULL || *buffer == NULL)
        return NULL;

    char *start = *buffer;
    char *posicao_delimitador;

    if ((posicao_delimitador = strpbrk(start, delimitador)) != NULL)
    {
        *posicao_delimitador = '\0';
        *buffer = posicao_delimitador + 1;
    }
    else
        *buffer = NULL;

    return start;
}
