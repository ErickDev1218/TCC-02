#include "Solution.hpp"
#include "PRD.hpp"
#include <iostream>

Solution::Solution(std::vector<int> solution, PRD* prd){
    this->solution = solution;
    this->isValid = prd->check_prd(this);
    this->fitness = this->calculateFitness();
}

int Solution::calculateFitness(){
    int fitness = 0;
    for(int i = 0; i < this->solution.size(); i++){
        fitness += this->solution[i];
    }
    if(!this->isValid) fitness += this->solution.size() * 5; // Penalidade

    return fitness;
}

void Solution::printSolution(){
    std::cout << "Solution: ";
    for(int i = 0; i < this->solution.size(); i++){
        std::cout << this->solution[i] << " ";
    }
    std::cout << "\nFitness: " << this->fitness << " - isValid:" << this->isValid << std::endl;
}

bool Solution::operator >(const Solution& other) const{
    return this->fitness > other.fitness;
}

bool Solution::operator <(const Solution& other) const{
    return this->fitness < other.fitness;
}

bool Solution::operator ==(const Solution& other) const{
    return this->fitness == other.fitness;
}

bool Solution::operator >=(const Solution& other) const{
    return this->fitness >= other.fitness;
}

bool Solution::operator <=(const Solution& other) const{
    return this->fitness <= other.fitness;
}