#include "Graph.hpp"
#include <iostream>

Graph::Graph(int numNodes,int numEdges, std::vector<std::pair<int, int>> edges, const std::string& gName) {
    this->numNodes = numNodes;
    this->numEdges = numEdges;
    this->nodes.reserve(numNodes);
    this->graphName = gName;

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

Graph::~Graph(){
    for(Node* u: this->nodes){
        delete u;
    }
    nodes.clear();
}

std::vector<int> Graph::getLabels(){
    std::vector<int> aux;
    for(Node* u: this->nodes){
        aux.push_back(u->label);
    }
    return aux;
}

void Graph::printGraph() {
    for (Node* node : this->nodes) {
        std::cout << "Label: " << node->label << ", Dominated: " << node->isDominated << ", Dominado por: " << node->dominatedFor << " -> Neighbors labels: ";
        for (auto neighbor : node->neighborhood) {
            std::cout << neighbor->label << " ";
        }
        std::cout << std::endl;
    }

}
