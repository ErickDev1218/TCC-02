#include "Graph.hpp"
#include <iostream>

Graph::Graph(int numNodes, std::vector<std::pair<int, int>> edges) {
    this->numNodes = numNodes;
    this->nodes.reserve(numNodes);
    
    for (int i = 0; i < numNodes; i++) {
        Node* newNode = new Node();
        this->nodes.push_back(newNode);
    }

    for (std::pair<int,int> edge : edges) {
        int u = edge.first;
        int v = edge.second;
        this->nodes[u]->neighborhood.push_back(this->nodes[v]);
        this->nodes[v]->neighborhood.push_back(this->nodes[u]);
    }
}

// void Graph::printGraph() {
//     for (auto node : this->nodes) {
//         std::cout << "Node " << node->id << " (Label: " << node->label << ", Dominated: " << node->isDominated << ") -> Neighbors: ";
//         for (auto neighbor : node->neighborhood) {
//             std::cout << neighbor->id << " ";
//         }
//         std::cout << std::endl;
//     }
// }