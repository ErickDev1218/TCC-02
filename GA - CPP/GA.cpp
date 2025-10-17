#include "GA.hpp"
#include "Solution.hpp"
#include <random>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream> 

GeneticAlgorithm::GeneticAlgorithm(Graph* g, int popFactor, int tournSize, int stagnant,float mutRate, float eleSize, float crosRate, int maxGenerations) {

    this->mutationRate = mutRate;
    this->populationSize = g->numNodes * popFactor;
    this->elitismSize = static_cast<std::size_t>(std::floor(this->populationSize * eleSize));
    this->crossoverRate = crosRate;
    this->maxGenerations = maxGenerations;
    this->maxStagnant = stagnant;
    this->tournamentSize = tournSize;
    
    this->g = g;
    this->prd = new PRD(this->g);
    this->population = this->initializePopulation();
    
}


void GeneticAlgorithm::gaFlow() {
    std::vector<Solution*> currentPop = this->population;
    Solution* best = currentPop[0];

    int gen = 0;
    for(int i = 0; i < maxGenerations; i++){

        std::cout << "Geracao " << i+1 << std::endl;
        GeneticAlgorithm::printSolutions(currentPop);
        std::cout << std::endl;
        // Selection
        std::vector<std::pair<Solution*, Solution*>> select = GeneticAlgorithm::tournamentSelection(currentPop);

        // Crossover
        std::vector<Solution*> newPop  = GeneticAlgorithm::onePointCrossover(select);

        // Mutation
        GeneticAlgorithm::randomMutation(newPop);

        // Elitism
        currentPop = GeneticAlgorithm::defaultElitism(currentPop, newPop);

        if(*currentPop[0] < *best) {
            best = currentPop[0];
            gen = 0;
        }else {
            gen++;
            if(gen >= this->maxStagnant) break; 
        }
    }
    // Por fim, a populacao final é salva
    this->population = currentPop;

    std::cout << "Melhor solução encontrada globalmente" << std::endl;
    GeneticAlgorithm::printSingleSolution(best);
    std::cout << std::endl;

}

// OPERATORS

// Selection
std::vector<std::pair<Solution*, Solution*>> GeneticAlgorithm::tournamentSelection(std::vector<Solution*> population) {
    std::vector<std::pair<Solution*, Solution*>> selectedPairs;
    for(int i = 0; i < population.size() / 2; i++){
        std::vector<Solution*> auxiliary = population;
        // Gerador aleatório
        std::random_device rd;
        std::mt19937 gen(rd());
        
        // Embaralha o vetor
        std::shuffle(auxiliary.begin(), auxiliary.end(), gen);
    
        Solution* first = GeneticAlgorithm::findMinimal(auxiliary, this->tournamentSize);
        
        Solution* second = GeneticAlgorithm::findMinimal(auxiliary, this->tournamentSize);
        
    
        selectedPairs.push_back(std::make_pair(first, second));
    }
    return selectedPairs;
}

// Crossover
std::vector<Solution*> GeneticAlgorithm::onePointCrossover(std::vector<std::pair<Solution*, Solution*>> pop) {

    std::vector<Solution*> result;

    for(std::pair<Solution*, Solution*> pair : pop){
        Solution* dad = pair.first;
        Solution* mom = pair.second;
        
        // GeneticAlgorithm::printVectorGA(dad->solution, mom->solution);

        int solutionLength = dad->solution.size();

        // Criar gerador de números aleatórios
        std::random_device rd;  // semente baseada em hardware
        std::mt19937 gen(rd()); // gerador Mersenne Twister

        // Distribuição uniforme entre 1 e N-1
        std::uniform_int_distribution<> dis(1, solutionLength - 2);

        int randomIndex = dis(gen); // gera o número aleatório
        // std::cout << "Random index is: " << randomIndex << std::endl;
        
        std::vector<int> firstChild;
        std::vector<int> secondChild; 

        for(int i = 0; i < randomIndex; i++){
            firstChild.push_back(dad->solution[i]);
            secondChild.push_back(mom->solution[i]);
        }

        for(int i = randomIndex; i < solutionLength; i++){
            firstChild.push_back(mom->solution[i]);
            secondChild.push_back(dad->solution[i]);
        }
        result.push_back(new Solution(firstChild, this->prd));
        result.push_back(new Solution(secondChild, this->prd));
    }
    return result;
}


// Mutation
void GeneticAlgorithm::randomMutation(std::vector<Solution*>& pop){
    // Geradores aleatórios 
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::uniform_int_distribution<> disInt(0, 2);


    for (Solution*& sol : pop) {
        bool changed = false;
        // Para todo gene, verifique a chance de mutação
        for (int k = 0; k < sol->solution.size(); k++) {
            if (dis(gen) < this->mutationRate) {
                sol->solution[k] = disInt(gen);
                changed = true;
            }
        }
        // Solucao alterada, necessario recalcular
        if (changed) {
            sol = this->fixElement(sol);
        }
    }
}

//Elitism
std::vector<Solution*> GeneticAlgorithm::defaultElitism(std::vector<Solution*>& current, std::vector<Solution*>& newPop) {

    std::vector<Solution*> result;

    // Ordena os vetores de solução
    std::sort(current.begin(), current.end(), [](Solution* a, Solution* b) { return *a < *b;}); 
    std::sort(newPop.begin(), newPop.end(), [](Solution* a, Solution* b) { return *a < *b;});
    // Copia os melhores da geracao passada 
    // e libera o restante da memoria
    for(int i = 0; i < current.size(); i++){ 
        if(i < this->elitismSize){
            result.push_back(current[i]); 
        }else{
            delete current[i];
        }
    } 
    // Copia o restante dos melhores da geracao atual
    // e libera o restante da memoria
    int res = newPop.size() - this->elitismSize;
    
    for(int i = 0; i < newPop.size(); i++){ 
        if(i < res){
            result.push_back(newPop[i]);
        }else {
            delete newPop[i];
        }
        
    } 
    return result;
}


// Auxiliary functions

Solution* GeneticAlgorithm::findMinimal(std::vector<Solution*>& preSelected, int toSee) {
    Solution* minimal = preSelected[0];
    int indexToRemove = 0;
    for(int i = 1; i < toSee; i++){
        if(*preSelected[i] < *minimal){ // comparando objetos, não ponteiros
            minimal = preSelected[i];
            indexToRemove = i;
        }
    }
    preSelected.erase(preSelected.begin() + indexToRemove);
    return minimal;
}

Solution* GeneticAlgorithm::fixElement(Solution* element){
    Solution* aux = new Solution(element->solution, this->prd);
    delete element;
    return aux;
}

std::vector<Solution*> GeneticAlgorithm::initializePopulation(){
    
    std::vector<Solution*> aux = this->prd->randomized_initialization();
    aux.push_back(this->prd->greedy_initialization());
    
    for(int i = aux.size(); i < this->populationSize; i++){
        aux.push_back(this->prd->random_solution());
    }

    return aux;
}

void GeneticAlgorithm::printVectorGA(std::vector<int> x, std::vector<int> y){
    std::cout << "Dad -> { " ;
    for(int i = 0; i < x.size(); i++){
        std::cout << x[i];

        if(i == x.size() - 1){
            std::cout << " } - ";
        }else{
            std::cout << ", ";
        }
    }

    std::cout << " Mom -> { " ;
    for(int i = 0; i < y.size(); i++){
        std::cout << y[i];

        if(i == y.size() - 1){
            std::cout << " }";
        }else{
            std::cout << ", ";
        }
    }

    std::cout << std::endl;
}

void GeneticAlgorithm::printSolutions(std::vector<Solution*>& pop) {
    for(Solution * ptr : pop) {
        std::cout << "Fitness: " << ptr->fitness << " - isValid: " << (ptr->isValid ? "Yes" : "No") << std::endl;
        std::cout << "Solution: ";
        for(int i = 0; i < ptr->solution.size(); i++){
            std::cout << ptr->solution[i] << " ";

            if(i == ptr->solution.size() - 1){
                std::cout << std::endl << std::endl;
            }
        }
    }
}

void GeneticAlgorithm::printSingleSolution(Solution* ptr){
    std::cout << "Fitness: " << ptr->fitness << " - isValid: " << (ptr->isValid ? "Yes" : "No") << std::endl;
    std::cout << "Solution: ";
    for(int i = 0; i < ptr->solution.size(); i++){
        std::cout << ptr->solution[i] << " ";

        if(i == ptr->solution.size() - 1){
            std::cout << std::endl << std::endl;
        }
    }
}