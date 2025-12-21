#ifndef RESULT_HPP
#define RESULT_HPP
#include <iostream>

class Result {
    public:
        std::string graph_name = "";
        int node_count = -1;
        int edge_count = -1;
        float graph_density = -1.0;
        int fitness = -1;
        double elapsed_time = -1;

        Result(std::string gn, int nc, int ec, float gd, int f, double et){
            this->graph_name = gn;
            this->node_count = nc;
            this->edge_count = ec;
            this->graph_density = gd;
            this->fitness = f;
            this->elapsed_time = et;
        }
};

#endif