#ifndef SOLUTION_HPP
#define SOLUTION_HPP
#include <vector>

class PRD; // Dependencia circular

class Solution {
    public:
        std::vector<int> solution;
        bool isValid;
        int fitness;
        
        Solution(std::vector<int> solution, PRD* prd);
        ~Solution() = default;
        
        void printSolution();
        int calculateFitness();

        bool operator >(const Solution& other) const;
        bool operator <(const Solution& other) const;
        bool operator ==(const Solution& other) const;
        bool operator >=(const Solution& other) const;
        bool operator <=(const Solution& other) const;
};








#endif