#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "../models/header.h"
#include "../models/record.h"

/*
    bin_to_text:
        This function reads a binary file and prints all its valid records
        to the standard output in a human-readable format.

        Args:
            No args.

        Return:
            SUCCESS (0) if successful, or -1 if an error occurs.
*/
int bin_to_text();

/*
    csv_to_bin:
        This function reads a CSV file, parses its contents, and writes them
        into a formatted binary file. It also calculates the total number of
        unique stations and unique station pairs.

        Args:
            No args.

        Return:
            SUCCESS (0) if successful, or -1 if an error occurs.
*/
int csv_to_bin();

/*
    read_binary_header:
        This function allocates memory for a new Header and reads its fields
        directly from the beginning of the given binary file.

        Args:
            - bin_file: pointer to the binary file to be read.

        Return:
            Pointer to the allocated and populated Header, or NULL on failure.
*/
Header *read_binary_header(FILE *bin_file);

/*
    save_header:
        This function rewinds the file pointer to the beginning and writes
        the provided Header structure into the binary file.

        Args:
            - bin_file: pointer to the target binary file.
            - header: pointer to the Header structure to be written.

        Return:
            No return.
*/
void save_header(FILE *bin_file, Header *header);

/*
    read_header:
        This function reads the header data from the binary file into an
        already allocated Header structure.

        Args:
            - bin_file: pointer to the binary file.
            - header: pointer to the existing Header structure.

        Return:
            0 if successful, -1 otherwise.
*/
int read_header(FILE *bin_file, Header *header);

/*
    read_record:
        This function reads a single complete record from the binary file,
        handling fixed-size fields, variable-length strings, and skipping
        padding bytes to respect the fixed RECORD_SIZE.

        Args:
            - bin_file: pointer to the binary file.
            - record: pointer to the Record structure to store the data.

        Return:
            0 if successful, -1 if reading fails or EOF is reached.
*/
int read_record(FILE *bin_file, Record *record);

/*
    save_record:
        This function writes only the fixed-size base fields of a record
        to the binary file.

        Args:
            - bin_file: pointer to the target binary file.
            - record: pointer to the Record to be saved.

        Return:
            No return.
*/
void save_record(FILE *bin_file, Record *record);

/*
    save_record_to_bin:
        This function writes a complete record to the binary file, including
        fixed fields, variable strings, and automatically appends '$' padding
        to ensure the written block matches the exact RECORD_SIZE.

        Args:
            - bin_file: pointer to the target binary file.
            - record: pointer to the Record to be completely saved.

        Return:
            No return.
*/
void save_record_to_bin(FILE *bin_file, Record *record);

/*
    generate_graph:
        This function builds and prints an adjacency list for a directed graph
        representing the train/subway network.

        Args:
            No args.

        Return:
            0 if successful, or -1 if an error occurs.
*/
int generate_graph();

/*
    shortest_path:
        This function applies Dijkstra's algorithm to find and print the 
        shortest path between an origin and a destination station.

        Args:
            No args.

        Return:
            0 if successful, or -1 if an error occurs.
*/
int shortest_path();

/*
    minimum_spanning_tree:
        This function applies Prim's algorithm to find the Minimum Spanning Tree (MST)
        of the undirected station network, then prints its pre-order traversal (DFS).

        Args:
            No args.

        Return:
            0 if successful, or -1 if an error occurs.
*/
int minimum_spanning_tree();


int count_cycles();
#endif