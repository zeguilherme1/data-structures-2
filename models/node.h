#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <cstdlib>

#include <string>
#include <vector>
#include <set>

struct Node {
    std::string next_station_name;
    int next_station_distance;
    std::set<std::string> linhas;
};




#endif