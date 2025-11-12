#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <random>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>

class Graph {
private:
    std::unordered_map<size_t, std::vector<size_t>> adjList;
    
public:	
    Graph() = default;
    Graph(const std::string& filename);

    void addVertex(size_t source);
    void addEdge(size_t source, size_t destination);
 	
    [[nodiscard]] size_t getOrder() const noexcept;
    [[nodiscard]] size_t getSize() const noexcept;
    [[nodiscard]] float getDensity() const noexcept;
    [[nodiscard]] size_t getVertexDegree(size_t vertex) const;
    [[nodiscard]] size_t getMinDegree() const;
    [[nodiscard]] size_t getMaxDegree() const;
    [[nodiscard]] const std::vector<size_t>& getNeighbors(size_t vertex) const;
    
    [[nodiscard]] bool edgeExists(size_t u, size_t v) const;
    [[nodiscard]] bool vertexExists(size_t vertex) const;	

    void deleteVertex(size_t vertex);

    [[nodiscard]] std::unordered_set<size_t> getIsolatedVertices() const;
    [[nodiscard]] std::unordered_set<size_t> getVertices() const;
    
    friend std::ostream& operator<< (std::ostream& os, const Graph& graph);

    [[nodiscard]] size_t chooseRandomVertex() const;

};

#endif