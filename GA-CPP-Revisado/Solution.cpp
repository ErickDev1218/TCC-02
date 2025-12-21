#include "Solution.hpp"
#include "PRD.hpp"


// ok - atilio
Solution::Solution(std::vector<int> solution, PRD* prd){
    this->solution = solution;
    
    /*this->isValid = prd->checkPRD(this);
    while(!isValid){
        prd->fixSolution(this); // Fix his solution
        bool test = prd->checkPRD(this);
        this->isValid = test;
    }*/
    
    prd->fixSolution(this);
    //this->isValid = true;
    this->fitness = this->calculateFitness();
}

// ok - atilio
int Solution::calculateFitness(){
    int fitness = 0;
    for(int i = 0; i < this->solution.size(); i++){
        fitness += this->solution[i];
    }
    //if(!this->isValid) fitness += this->solution.size() * 5; // Penality

    return fitness;
}

// ok - atilio
bool Solution::operator >(const Solution& other) const{
    return this->fitness > other.fitness;
}
// ok - atilio
bool Solution::operator <(const Solution& other) const{
    return this->fitness < other.fitness;
}
// ok - atilio
bool Solution::operator ==(const Solution& other) const{
    return this->fitness == other.fitness;
}
// ok - atilio
bool Solution::operator >=(const Solution& other) const{
    return this->fitness >= other.fitness;
}
// ok - atilio
bool Solution::operator <=(const Solution& other) const{
    return this->fitness <= other.fitness;
}

// ok - atilio
void Solution::printSolution(){
    std::cout << "Solution: ";
    for(int i = 0; i < this->solution.size(); i++){
        std::cout << this->solution[i] << " ";
    }
    //std::cout << "\nFitness: " << this->fitness << " - isValid:" << this->isValid << std::endl;
    std::cout << "\nFitness: " << this->fitness << std::endl;
}
