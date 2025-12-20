#ifndef PRD_HPP
#define PRD_HPP
#include "Graph.hpp"
#include <algorithm>
#include <queue>
#include <functional>
#include <iostream>
#include <random>

class Solution; // Circular dependency   
 

class PRD {
    public:
        Graph* graph;
        PRD(Graph* g);
        ~PRD() = default;

        bool checkPRD(Solution* sol);
        Solution* greedyInitialization();
        Solution* randomSolution();
        void fixSolution(Solution* s);
        void reduceWeight(Solution* s); 
        std::vector<Solution*> randomizedInitialization(int populationSize);
        
    private:
        void resetGraph(std::vector<int> s);
        void restartGraph();
};




#endif




