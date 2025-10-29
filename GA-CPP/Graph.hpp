#ifndef GRAPH_CPP
#define GRAPH_CPP
#include <vector>
#include "Node.hpp"
#include <utility>
#include <string>


class Graph{
    public:
        int numNodes;
        std::string graphName;
        std::vector<Node*> nodes;

        Graph(int numNodes, std::vector<std::pair<int, int>> edges, std::string gName);
        ~Graph();
        
        std::vector<int> getLabels();
        // void label_nodes(vector<int> labels);
        // vector<boolean> get_dominated_status();
        // void reset_labels_and_dominance();

        void printGraph();
};





#endif
