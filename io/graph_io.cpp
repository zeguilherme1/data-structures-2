#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>

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

    if (read_header(bin_file, temp_header) == -1 || temp_header->status != '1')
    {
        printf("Falha na execução da funcionalidade.\n");
        free(temp_header);
        fclose(bin_file);
        return -1;
    }

    std::unordered_map<int, std::string> station_dict;

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

    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        Record *rec = read_rrn_record(bin_file, i);
        if (rec != NULL)
        {
            if (rec->removed == FALSE && rec->station_name != NULL)
            {
                std::string origin_name = std::string(rec->station_name);

                if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end())
                {
                    std::string dest_name = station_dict[rec->next_station_code];

                    graph[origin_name][dest_name].next_station_distance = rec->next_station_distance;
                    if (rec->line_name != NULL)
                    {
                        graph[origin_name][dest_name].linhas.insert(std::string(rec->line_name));
                    }
                }
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

int shortest_path() {
    char bin_filename[100];
    char dummy1[100];
    char dummy_origem[100], dummy_destino[100];
    char origem[100], destino[100];

    if (scanf("%s", bin_filename) != 1)
    {
        printf("Falha na execução da funcionalidade.\n");
        return -1;
    }
    
    scanf("%s", dummy1);
    
    if (strstr(dummy1, ".bin") != NULL) {
        scanf("%s", dummy_origem);
    } else {
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

    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        if (read_record(bin_file, rec) == -1) break;

        if (rec->removed == FALSE && rec->station_name != NULL)
        {
            station_dict[rec->station_code] = std::string(rec->station_name);
        }
        
        if (rec->station_name) { free(rec->station_name); rec->station_name = NULL; }
        if (rec->line_name) { free(rec->line_name); rec->line_name = NULL; }
    }

    std::map<std::string, std::map<std::string, Node>> graph;

    fseek(bin_file, data_offset, SEEK_SET);
    for (int i = 0; i < temp_header->nextRRN; i++)
    {
        if (read_record(bin_file, rec) == -1) break;

        if (rec->removed == FALSE && rec->station_name != NULL)
        {
            std::string origin_name = std::string(rec->station_name);

            if (rec->next_station_code != -1 && station_dict.find(rec->next_station_code) != station_dict.end())
            {
                std::string dest_name = station_dict[rec->next_station_code];

                graph[origin_name][dest_name].next_station_distance = rec->next_station_distance;
                if (rec->line_name != NULL)
                {
                    graph[origin_name][dest_name].linhas.insert(std::string(rec->line_name));
                }
            }
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

        if (rec->station_name) { free(rec->station_name); rec->station_name = NULL; }
        if (rec->line_name) { free(rec->line_name); rec->line_name = NULL; }
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
    
    std::set<std::pair<int, std::string>> pq; 

    for (auto const& [u, edges] : graph) {
        dist[u] = INF;
        for (auto const& [v, info] : edges) {
            dist[v] = INF;
        }
    }

    dist[origem_str] = 0;
    pq.insert({0, origem_str});

    while (!pq.empty()) {
        auto [d, u] = *pq.begin();
        pq.erase(pq.begin());

        if (d > dist[u]) continue; 
        if (u == destino_str) break; 
        if (graph.find(u) == graph.end()) continue;

        for (auto const& [v, info] : graph.at(u)) {
            int peso = info.next_station_distance;
            int nova_distancia = dist[u] + peso;

            if (nova_distancia < dist[v]) {
                pq.erase({dist[v], v});
                dist[v] = nova_distancia;
                prev[v] = u;
                pq.insert({dist[v], v});
            } 
            else if (nova_distancia == dist[v]) {
                if (prev.find(v) != prev.end() && u < prev[v]) {
                    prev[v] = u;
                }
            }
        }
    }

    if (dist[destino_str] == INF) {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return 0;
    }

    std::vector<std::string> path;
    std::string atual = destino_str;

    while (atual != "") {
        path.push_back(atual);
        if (atual == origem_str) break;
        atual = prev[atual];
    }

    std::reverse(path.begin(), path.end());

    if (path.front() != origem_str) {
        printf("Não existe caminho entre as estações solicitadas.\n");
        return 0;
    }

    int estacoes_percorridas = path.size() - 1;
    printf("Numero de estacoes que serao percorridas: %d\n", estacoes_percorridas);
    printf("Distancia que sera percorrida: %d\n", dist[destino_str]);

    for (size_t i = 0; i < path.size(); i++) {
        std::cout << path[i];
        if (i < path.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;

    return 0;
}