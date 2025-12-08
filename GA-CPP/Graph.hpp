#ifndef GRAPH_CPP
#define GRAPH_CPP
#include "Node.hpp"
#include <vector>
#include <utility>
#include <string>

class Graph{
    public:
        int numNodes;
        int numEdges;
        std::string graphName;
        std::vector<Node*> nodes;

        Graph(int numNodes, int numEdges, std::vector<std::pair<int, int>> edges, const std::string& gName);
        ~Graph();
        
        std::vector<int> getLabels();
        void printGraph();
};

#endif