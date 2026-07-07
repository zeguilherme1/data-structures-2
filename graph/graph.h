#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include "../models/header.h"
#include "../models/record.h"


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

/*
    count_cycles:
        This function reads a binary station file and rebuilds it as a directed
        graph (regular line connections plus physical integration links), then
        runs a DFS from a given origin station to count how many distinct cycles
        lead back to that origin.
        Args:
            No args 
        Return:
            0 if successful, or -1 if an error occurs.
*/
int count_cycles();

#endif