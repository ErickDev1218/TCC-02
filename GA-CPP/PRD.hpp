#ifndef PRD_HPP
#define PRD_HPP
#include "Node.hpp"
#include "Graph.hpp"
#include <vector>
#include <algorithm>
#include <queue>
#include <functional>
#include <iostream>
#include <random>
// #include "Solution.hpp"

class Solution; // Dependencia circular
 

class PRD {
    public:
        Graph* graph;
        PRD(Graph* g);
        ~PRD() = default;

        Solution* greedy_initialization();
        Solution* random_solution();
        std::vector<Solution*> randomized_initialization();
        
        
        bool check_prd(Solution* sol);

    private:
        void restartGraph();
        void fixSolution();
};




#endif
