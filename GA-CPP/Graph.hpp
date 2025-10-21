#ifndef GRAPH_CPP
#define GRAPH_CPP
#include <vector>
#include "Node.hpp"
#include <utility>

class Graph{
    public:
        int numNodes;
        std::vector<Node*> nodes;

        Graph(int numNodes, std::vector<std::pair<int, int>> edges);
        ~Graph();
        
        // vector<int> getLabels(int u, int v);
        // void label_nodes(vector<int> labels);
        // vector<boolean> get_dominated_status();
        // void reset_labels_and_dominance();

        // void printGraph();
};




#endif