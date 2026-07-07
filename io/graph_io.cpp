#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <vector>
#include <functional>

#include "../constants.h"
#include "../models/header.h"
#include "../models/record.h"
#include "../models/node.h"
#include "../models/index.h"
#include "../search/search.h"
#include "../parser/tokenizer.h"
#include "../utils/utils.h"

#define INF 1e8 + 5

#include "io.h"

int generate_graph()
{
    char bin_filename[100];

    // Read the binary file name
    if (scanf("%s", bin_filename) != 1)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);

    if (bin_file == NULL)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    Header *temp_header = new_header();
    if (temp_header == NULL)
    {
        fclose(bin_file);
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    // Read and validate the file header
    if (read_header(bin_file, temp_header) == -1 || temp_header->status != '1')
    {
        printf("Falha na execução da funcionalidade.\n");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    std::unordered_map<int, std::string> station_dict;

    // Cache the station names mapped by their ID codes
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL)
        {
            if (rec->removed == FALSE && rec->station_name != NULL)
            {
                station_dict[rec->station_code] = std::string(rec->station_name);
            }
            free_record(&rec);
        }
    }

    std::map<std::string, std::map<std::string, Node>> graph;

    // Second pass to build the adjacency list representing the graph
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL)
        {
            if (rec->removed == FALSE && rec->station_name != NULL)
            {
                std::string origin_name = std::string(rec->station_name);

                // Add edges for standard connections
                if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end())
                {
                    std::string dest_name = station_dict[rec->next_station_code];

                    graph[origin_name][dest_name].next_station_distance = rec->next_station_distance;
                    if (rec->line_name != NULL)
                    {
                        graph[origin_name][dest_name].linhas.insert(std::string(rec->line_name));
                    }
                }

                // Add edges for physical integration between lines
                if (rec->station_integration_code != -1 && station_dict.find(rec->station_integration_code) != station_dict.end())
                {
                    std::string dest_name = station_dict[rec->station_integration_code];

                    if (origin_name != dest_name)
                    {
                        if (graph[origin_name][dest_name].linhas.empty())
                        {
                            graph[origin_name][dest_name].next_station_distance = 0;
                        }
                        graph[origin_name][dest_name].linhas.insert("Integração");
                    }
                }
            }
            free_record(&rec);
        }
    }

    if (graph.empty())
    {
        printf("Falha na execução da funcionalidade.\n");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    // Print the graph traversing the nested maps (automatically sorted)
    for (auto const &[origin, edges] : graph)
    {
        std::cout << origin;

        for (auto const &[dest, info] : edges)
        {
            std::cout << ", " << dest << ", " << info.next_station_distance;

            for (auto const &linha : info.linhas)
            {
                std::cout << ", " << linha;
            }
        }
        std::cout << std::endl;
    }

    free(temp_header);
    fclose(bin_file);
    return 0;
}

int shortest_path()
{
    char bin_filename[100];
    char dummy1[100];
    char dummy_origem[100], dummy_destino[100];
    char origem[100], destino[100];

    // Read the binary file name
    if (scanf("%s", bin_filename) != 1)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    scanf("%s", dummy1);

    // Dynamic parser to ignore injected index files during testing
    if (strstr(dummy1, ".bin") != NULL)
    {
        scanf("%s", dummy_origem);
    }
    else
    {
        strcpy(dummy_origem, dummy1);
    }

    scan_quote_string(origem);
    scanf("%s", dummy_destino);
    scan_quote_string(destino);

    std::string origem_str(origem);
    std::string destino_str(destino);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    Header *temp_header = new_header();
    if (temp_header == NULL)
    {
        fclose(bin_file);
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    // Read and validate the file header
    if (read_header(bin_file, temp_header) == -1 || temp_header->status != '1')
    {
        printf("Falha na execução da funcionalidade.\n");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    long data_offset = ftell(bin_file);
    std::unordered_map<int, std::string> station_dict;
    Record *rec = new_record();

    // Cache the station names mapped by their ID codes
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        if (read_record(bin_file, rec) == -1)
            break;

        if (rec->removed == FALSE && rec->station_name != NULL)
        {
            station_dict[rec->station_code] = std::string(rec->station_name);
        }

        if (rec->station_name)
        {
            free(rec->station_name);
            rec->station_name = NULL;
        }
        if (rec->line_name)
        {
            free(rec->line_name);
            rec->line_name = NULL;
        }
    }

    std::map<std::string, std::map<std::string, Node>> graph;

    fseek(bin_file, data_offset, SEEK_SET);

    // Second pass to build the adjacency list representing the graph
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        if (read_record(bin_file, rec) == -1)
            break;

        if (rec->removed == FALSE && rec->station_name != NULL)
        {
            std::string origin_name = std::string(rec->station_name);

            // Add edges for standard connections
            if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end())
            {
                std::string dest_name = station_dict[rec->next_station_code];

                graph[origin_name][dest_name].next_station_distance = rec->next_station_distance;
                if (rec->line_name != NULL)
                {
                    graph[origin_name][dest_name].linhas.insert(std::string(rec->line_name));
                }
            }

            // Add edges for physical integration between lines
            if (rec->station_integration_code != -1 && station_dict.find(rec->station_integration_code) != station_dict.end())
            {
                std::string dest_name = station_dict[rec->station_integration_code];

                if (origin_name != dest_name)
                {
                    if (graph[origin_name][dest_name].linhas.empty())
                    {
                        graph[origin_name][dest_name].next_station_distance = 0;
                    }
                    graph[origin_name][dest_name].linhas.insert("Integração");
                }
            }
        }

        if (rec->station_name)
        {
            free(rec->station_name);
            rec->station_name = NULL;
        }
        if (rec->line_name)
        {
            free(rec->line_name);
            rec->line_name = NULL;
        }
    }

    free_record(&rec);
    free(temp_header);
    fclose(bin_file);

    if (graph.empty() || graph.find(origem_str) == graph.end())
    {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return 0;
    }

    std::map<std::string, int> dist;
    std::map<std::string, std::string> prev;

    // Priority queue to traverse paths
    std::set<std::pair<int, std::string>> pq;

    // Initialize distances to infinity
    for (auto const &[u, edges] : graph)
    {
        dist[u] = INF;
        for (auto const &[v, info] : edges)
        {
            dist[v] = INF;
        }
    }

    dist[origem_str] = 0;
    pq.insert({0, origem_str});

    // Execute Dijkstra's shortest path algorithm
    while (!pq.empty())
    {
        auto [d, u] = *pq.begin();
        pq.erase(pq.begin());

        if (d > dist[u])
            continue;
        if (u == destino_str)
            break;
        if (graph.find(u) == graph.end())
            continue;

        for (auto const &[v, info] : graph.at(u))
        {
            int peso = info.next_station_distance;
            int nova_distancia = dist[u] + peso;

            // Relaxation step: found a strictly shorter path
            if (nova_distancia < dist[v])
            {
                pq.erase({dist[v], v});
                dist[v] = nova_distancia;
                prev[v] = u;
                pq.insert({dist[v], v});
            }
            // Tie-breaking rule for identical distances
            else if (nova_distancia == dist[v])
            {
                if (prev.find(v) != prev.end() && u < prev[v])
                {
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[destino_str] == INF)
    {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return 0;
    }

    std::vector<std::string> path;
    std::string atual = destino_str;

    // Reconstruct the shortest path
    while (atual != "")
    {
        path.push_back(atual);
        if (atual == origem_str)
            break;
        atual = prev[atual];
    }

    // Reverse the path to get Origin -> Destination order
    std::reverse(path.begin(), path.end());

    if (path.front() != origem_str)
    {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return 0;
    }

    // Print resulting path distances and nodes
    int estacoes_percorridas = path.size() - 1;
    printf("Numero de estacoes que serao percorridas: %d\n", estacoes_percorridas);
    printf("Distancia que sera percorrida: %d\n", dist[destino_str]);

    for (size_t i = 0; i < path.size(); i++)
    {
        std::cout << path[i];
        if (i < path.size() - 1)
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    return 0;
}

// Struct utilized safely inside the cpp file for Prim's algorithm
struct Edge
{
    int weight;
    std::string u;
    std::string v;

    // Overloaded operator inherently handles the tie-breaking rules required
    bool operator<(const Edge &other) const
    {
        if (weight != other.weight)
            return weight < other.weight;
        if (u != other.u)
            return u < other.u;
        return v < other.v;
    }
};

int minimum_spanning_tree()
{
    char bin_filename[100];
    char dummy1[100];
    char dummy_origin[100];
    char origin[100];

    // Read the binary file name
    if (scanf("%s", bin_filename) != 1)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    scanf("%s", dummy1);

    // Dynamic parser: ignores injected binary index files if present to safely read the origin field
    if (strstr(dummy1, ".bin") != NULL)
    {
        scanf("%s", dummy_origin);
    }
    else
    {
        strcpy(dummy_origin, dummy1);
    }

    scan_quote_string(origin);
    std::string origem_str(origin);

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    Header *temp_header = new_header();
    if (temp_header == NULL)
    {
        fclose(bin_file);
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    // Validate the consistency of the binary file header
    if (read_header(bin_file, temp_header) == -1 || temp_header->status != '1')
    {
        printf("Falha na execução da funcionalidade.\n");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    std::unordered_map<int, std::string> station_dict;

    // STEP 1: Cache Station IDs to their respective names using the proper read_rrn_record signature
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL)
        {
            if (rec->removed == FALSE && rec->station_name != NULL)
            {
                station_dict[rec->station_code] = std::string(rec->station_name);
            }
            free_record(&rec);
        }
    }

    // Adjacency list for an UNDIRECTED graph keeping track of the minimum weights
    std::map<std::string, std::map<std::string, int>> adj;

    // STEP 2: Graph Construction mapping bi-directional paths securely
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL)
        {
            if (rec->removed == FALSE && rec->station_name != NULL)
            {
                std::string origin_name = std::string(rec->station_name);

                // Add standard line edges mapping both ways since lines can be bidirectional
                if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end())
                {
                    std::string dest_name = station_dict[rec->next_station_code];
                    int dist = rec->next_station_distance;

                    // Store the route only if it is a new connection or if the distance is shorter than the current one
                    if (adj[origin_name].find(dest_name) == adj[origin_name].end() || dist < adj[origin_name][dest_name])
                    {
                        adj[origin_name][dest_name] = dist;
                        adj[dest_name][origin_name] = dist;
                    }
                }

                // Add physical integration paths mapping both ways with neutral distance (0)
                if (rec->station_integration_code != -1 && station_dict.find(rec->station_integration_code) != station_dict.end())
                {
                    std::string dest_name = station_dict[rec->station_integration_code];

                    if (origin_name != dest_name)
                    {
                        int dist = 0;
                        if (adj[origin_name].find(dest_name) == adj[origin_name].end() || dist < adj[origin_name][dest_name])
                        {
                            adj[origin_name][dest_name] = dist;
                            adj[dest_name][origin_name] = dist;
                        }
                    }
                }
            }
            free_record(&rec);
        }
    }

    free(temp_header);
    fclose(bin_file);

    // Abort if origin string doesn't have an established edge network to traverse
    if (adj.empty() || adj.find(origem_str) == adj.end())
    {
        printf("Falha na execução da funcionalidade.\n");
        return 0;
    }

    // STEP 3: Compute the Minimum Spanning Tree applying custom tie rules
    std::set<Edge> pq;
    std::set<std::string> visited;

    // We map one way for the resulting MST to build a directed tree properly rooted at the origin
    // Using a map ensures that the children are inherently sorted alphabetically by station name
    std::map<std::string, std::map<std::string, int>> mst;

    visited.insert(origem_str);

    // Insert all initial connections starting from the origin station
    for (auto const &[v, w] : adj[origem_str])
    {
        pq.insert({w, origem_str, v});
    }

    // Traverse the Prim tree considering custom rules mapped inside Edge struct
    while (!pq.empty())
    {
        Edge e = *pq.begin();
        pq.erase(pq.begin());

        // Ignore if the destination already belongs to the MST (prevents cycles)
        if (visited.find(e.v) != visited.end())
            continue;

        // Add the edge to the MST (Directed from parent to child) and mark as visited
        visited.insert(e.v);
        mst[e.u][e.v] = e.weight;

        // Search the neighbors of the new vertex and add them to the priority queue
        for (auto const &[next_v, w] : adj[e.v])
        {
            if (visited.find(next_v) == visited.end())
            {
                pq.insert({w, e.v, next_v});
            }
        }
    }

    // STEP 4: Render tree output printing its Depth-First Search (DFS) route
    std::set<std::string> visited_dfs;

    // Using recursive lambda pattern securely
    std::function<void(const std::string &)> dfs = [&](const std::string &u)
    {
        visited_dfs.insert(u);

        // mst[u] is a std::map, so it iterates through children strictly in alphabetical order
        for (auto const &[v, w] : mst[u])
        {
            if (visited_dfs.find(v) == visited_dfs.end())
            {
                std::cout << u << ", " << v << ", " << w << std::endl;
                dfs(v);
            }
        }
    };

    // Run DFS loop starting from the root of our directed tree
    dfs(origem_str);

    return 0;
}

void dfs_cycle_counter(
    const std::string &u,
    const std::string &origin,
    std::map<std::string, std::map<std::string, Node>> &graph,
    std::map<std::string, int> &cor,
    int &total_cycles)
{
    cor[u] = 1;

    for (const auto &edge : graph[u])
    {
        const std::string &v = edge.first;

        if (v == origin)
        {
            total_cycles++;
        }
        else if (cor[v] == 0)
        {
            dfs_cycle_counter(v, origin, graph, cor, total_cycles);
        }
    }

    cor[u] = 0;
}

int count_cycles()
{
    char arg1[100], arg2[100], arg3[100];
    char origin[100];

    
    if (scanf("%s", arg1) != 1) {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }
    
    scanf("%s", arg2);

    
    if (strstr(arg2, ".bin") != NULL) {
        scanf("%s", arg3); 
        scan_quote_string(origin); 
    } else {
        
        scan_quote_string(origin); 
    }
    
    std::string origem_str(origin);
    char* bin_filename = arg1;

    FILE *bin_file = fopen(bin_filename, READ_BINARY_MODE);
    if (bin_file == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }

    Header *temp_header = new_header();
    if (read_header(bin_file, temp_header) == -1 || temp_header->status != '1') {
        printf("Falha na execução da funcionalidade.\n");
        if (temp_header) free(temp_header);
        fclose(bin_file);
        return -1;
    }

    std::unordered_map<int, std::string> station_dict;

    
    for (int i = 0; i < temp_header->nextRRN; i++) {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL) {
            if (rec->removed == FALSE && rec->station_name != NULL) {
                station_dict[rec->station_code] = std::string(rec->station_name);
            }
            free_record(&rec);
        }
    }

    std::map<std::string, std::map<std::string, Node>> graph;

    
    for (int i = 0; i < temp_header->nextRRN; i++) {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL) {
            if (rec->removed == FALSE && rec->station_name != NULL) {
                std::string origin_name = std::string(rec->station_name);

                if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end()) {
                    std::string dest_name = station_dict[rec->next_station_code];

                    if (origin_name != dest_name) {
                        graph[origin_name][dest_name].next_station_distance = rec->next_station_distance;
                        if (rec->line_name != NULL) graph[origin_name][dest_name].linhas.insert(std::string(rec->line_name));
                    }
                }

                if (rec->station_integration_code != -1 && station_dict.find(rec->station_integration_code) != station_dict.end()) {
                    std::string dest_name = station_dict[rec->station_integration_code];

                    if (origin_name != dest_name) {
                        if (graph[origin_name][dest_name].linhas.empty()) graph[origin_name][dest_name].next_station_distance = 0;
                        graph[origin_name][dest_name].linhas.insert("Integração");
                    }
                }
            }
            free_record(&rec);
        }
    }

    free(temp_header);
    fclose(bin_file);

    if (graph.find(origem_str) == graph.end()) {
        printf("Quantidade de ciclos: -1\n");
        return 0;
    }

    
    int total_cycles = 0;
    std::map<std::string, int> cor; 

    
    dfs_cycle_counter(origem_str, origem_str, graph, cor, total_cycles);

    

    if (total_cycles > 0) {
        printf("Quantidade de ciclos: %d\n", total_cycles);
    } else {
        printf("Quantidade de ciclos: -1\n");
    }

    return 0;
}