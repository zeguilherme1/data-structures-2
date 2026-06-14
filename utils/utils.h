#ifndef UTILS_H
#define UTILS_H

// * Debug Utils

/*
    BinarioNaTela:
        This function reads a binary file and calculates a hash based on its bytes.
        It is primarily used for testing and validation purposes on the runcodes platform.

        Args:
            - arquivo: name of the binary file to be evaluated.

        Return:
            No return.
*/
void BinarioNaTela(char *arquivo);

// * String Utils

/*
    integer_or_null:
        This function receives a string and converts it to an integer.
        If the string is empty, NULL, or missing, it returns a default null representation (-1).

        Args:
            - str: string to be parsed.

        Return:
            Integer value or -1 if null/empty.
*/
int integer_or_null(char *str);

/*
    print_int:
        This function prints an integer field to the standard output.
        If the value is -1, it prints the string "NULO " instead.

        Args:
            - value: integer to be printed.

        Return:
            No return.
*/
void print_int(int value);

/*
    print_string:
        This function prints a string field restricted to a specified size.
        If the given size is 0, it prints the string "NULO ".

        Args:
            - str: string to be printed.
            - size: size of the string.

        Return:
            No return.
*/
void print_string(char *str, int size);

/*
    matches_integer:
        This function compares a string representing search criteria against an integer field.
        It correctly handles the "NULO" criteria edge case.

        Args:
            - criteria: string containing the search criteria.
            - field: integer field to be compared.

        Return:
            0 if they match, -1 otherwise.
*/
int matches_integer(char *criteria, int field);

/*
    matches_string:
        This function compares a string criteria against a string field of a specific size.
        It handles "NULO" criteria and ensures exact string matches ignoring garbage data.

        Args:
            - criteria: string containing the search criteria.
            - field: string field from the record.
            - size: size of the record's string field.

        Return:
            0 if they match, -1 otherwise.
*/
int matches_string(char *criteria, char *field, int size);

/*
    clean_string:
        This function removes trailing carriage returns (\r) and newlines (\n) from a string.

        Args:
            - str: string to be cleaned.

        Return:
            No return.
*/
void clean_string(char *str);

/*
    scan_quote_string:
        This function reads a string from standard input that might be enclosed in quotes.
        It handles leading spaces, the specific keyword "NULO", and ignores the quotes themselves.

        Args:
            - str: buffer pointer to store the read string.

        Return:
            No return.
*/
void scan_quote_string(char *str);

#endif