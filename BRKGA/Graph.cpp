#include "Graph.h"

#define DEBUG 0

Graph::Graph(const std::string& filename) {
	
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Error opening the file!" + filename);
    }

    int num_vertex, num_edges; 
    file >> num_vertex >> num_edges; // primeira linha
    for(int i = 0; i < num_vertex; i++) addVertex(i);

    int u, v;
    while(file >> u >> v){
        addEdge(u, v);
    }
    
    file.close();
}

void Graph::addVertex(size_t source) {
	if (!vertexExists(source)) {
        adjList[source] = {};
    }
}

void Graph::addEdge(size_t source, size_t destination) {
    // Ignore self-loops
	if (source == destination) { return; }

    // Check if the endpoints exist
    if (!vertexExists(source) || !vertexExists(destination)) {
        throw std::runtime_error("error: endpoint of edge does not exist (function addEdge)");
    }

    // Ignore multiple edges
    if (edgeExists(source, destination)) { return; }

    this->adjList[source].push_back(destination);
    this->adjList[destination].push_back(source);            
}

size_t Graph::getOrder() const noexcept { return adjList.size(); }

size_t Graph::getSize() const noexcept { 
    size_t count = 0;
    for (const auto &[vertex, neighbors] : adjList) {
      count += neighbors.size();
    }
    // Each edge is counted two times 
    return count / 2;
}

size_t Graph::getVertexDegree(size_t vertex) const {
    if (!vertexExists(vertex)) {
        std::string str = "Vertex " + std::to_string(vertex) + " does not exists in the graph";
        throw std::out_of_range(str);
    }
    return adjList.at(vertex).size();
}

size_t Graph::getMinDegree() const { 
    if (adjList.empty()) {
        throw std::runtime_error("error: empty graph does not have minimum degree");
    }
	
 	size_t minDegree = std::numeric_limits<int>::max();
    
    for (const auto& [vertex, neighbors] : adjList) {
        size_t temp = neighbors.size();
        if (minDegree > temp) {
            minDegree = temp;
        }
    }
            
    return minDegree;
}

size_t Graph::getMaxDegree() const { 
    if (adjList.empty()) {
        throw std::runtime_error("error: empty graph does not have minimum degree");
    }
	
 	size_t maxDegree = 0;
    
    for (const auto& [vertex, neighbors] : adjList) {
        size_t temp = neighbors.size();
        if (maxDegree < temp) {
            maxDegree = temp;
        }
    }
            
    return maxDegree;
}

const std::vector<size_t>& Graph::getNeighbors(size_t vertex) const { return this->adjList.at(vertex); }

bool Graph::vertexExists(size_t vertex) const { return adjList.find(vertex) != adjList.end(); }

bool Graph::edgeExists(size_t u, size_t v) const {
    if(!vertexExists(u) || !vertexExists(v)) {
        throw std::runtime_error("error: endpoint of edge does not exists");
    }
    return std::find(adjList.at(u).begin(), adjList.at(u).end(), v) != adjList.at(u).end();
}

void Graph::deleteVertex(size_t vertex) {
    if (!vertexExists(vertex)) {
        throw std::runtime_error("Vertex does not exist (function deleteVertex)");
    }
    // First, delete all the edges incident with 'vertex'
    for(const size_t& neighbor : adjList[vertex]) {
        for(auto it = adjList[neighbor].begin(); it != adjList[neighbor].end(); ++it) {
            if(*it == vertex) {
                adjList[neighbor].erase(it);
                break;
            }
        }
    }
    // Then, delete 'vertex'
    adjList.erase(vertex);
}

float Graph::getDensity() const noexcept {
    return static_cast<float>(getOrder() * 2) / (getOrder() * (getOrder() - 1)); 
}

std::unordered_set<size_t> Graph::getIsolatedVertices() const {
    std::unordered_set<size_t> vertices;
    for (const auto &[vertex, _] : adjList) {
      if(getVertexDegree(vertex) == 0) {
        vertices.insert(vertex);
      }
    }
    return vertices;
}

std::unordered_set<size_t> Graph::getVertices() const {
    std::unordered_set<size_t> vertices;
    for (const auto &[vertex, _] : adjList) {
      vertices.insert(vertex);
    }
    return vertices;
  }

std::ostream& operator<< (std::ostream& os, const Graph& graph) {
    for (const auto& [u, v] : graph.adjList) {
        size_t vertex = u;  

        os << vertex << " ----> ";
        for (const auto& neighbor : v) { 
            os << neighbor << " ";  
    	}
    	        
        os << '\n';
    	
 	}
 	   
    return os;
}

size_t Graph::chooseRandomVertex() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::unordered_set<size_t> vertices = getVertices();

    if (vertices.empty()) {
      throw std::runtime_error("Graph is empty, cannot choose a vertex.");
    }
    std::uniform_int_distribution<> distrib(0, vertices.size() - 1);
    auto it = vertices.begin();
    std::advance(it, distrib(gen));
    return *it;
};
