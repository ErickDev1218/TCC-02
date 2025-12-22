#include "GA.hpp"
#include "Solution.hpp"

// ok atilio
GeneticAlgorithm::GeneticAlgorithm(Graph* g, int popFactor, int tournSize, 
    int stagnant,float mutRate, float eleSize, int maxGenerations) : gen(std::random_device{}()),dis(0.1, 1.0), disInt(0, 1) {

    this->mutationRate = mutRate;
    this->populationSize = g->numNodes / popFactor;
    this->elitismSize = static_cast<std::size_t>(std::floor(this->populationSize * eleSize));
    this->maxGenerations = maxGenerations;
    this->maxStagnant = stagnant;
    this->tournamentSize = tournSize;

    this->g = g;
    this->prd = new PRD(this->g);
    this->population = this->initializePopulation();
}

// ok atilio
GeneticAlgorithm::~GeneticAlgorithm(){
    delete this->prd;
    for(Solution* s: population){
        delete s;
    }
    population.clear();
}

Result GeneticAlgorithm::gaFlow() {

    std::vector<Solution*> currentPop = this->population;  

    Result res = Result(
        this->g->graphName,
        this->g->numNodes, 
        this->g->numEdges,
        static_cast<float>(2 * this->g->numNodes) / (this->g->numNodes * (this->g->numEdges - 1)),
        std::numeric_limits<int>::max(),
        0.0
    );

    const double TIME_LIMIT = 900.0;

    auto begin = std::chrono::high_resolution_clock::now();

    int gen = 0;
    for(int i = 0; i < this->maxGenerations && gen < this->maxStagnant; i++){

        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - begin
        ).count();

        if (elapsed > TIME_LIMIT) {
            res.elapsed_time = TIME_LIMIT;
            this->population = currentPop;
            return res;
        }
        // Selection
        std::vector<std::pair<Solution*, Solution*>> select = GeneticAlgorithm::tournamentSelection(currentPop);

        // Crossover
        std::vector<Solution*> newPop  = GeneticAlgorithm::onePointCrossover(select);

        // Mutation
        GeneticAlgorithm::randomMutation(newPop);

        // Elitism
        currentPop = GeneticAlgorithm::defaultElitism(currentPop, newPop);
        if(currentPop[0]->fitness < res.fitness) {
            res.fitness = currentPop[0]->fitness;
            gen = 0;
        }else {
            gen++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    res.elapsed_time = std::chrono::duration<double>(end - begin).count();

    this->population = currentPop;
    
    return res;
}

// OPERATORS

// ok atilio
// Selection
std::vector<std::pair<Solution*, Solution*>> GeneticAlgorithm::tournamentSelection(std::vector<Solution*> population) {
    std::vector<std::pair<Solution*, Solution*>> selectedPairs;
    for(int i = 0; i < std::ceil(population.size()) / 2; i++){
        std::vector<Solution*> auxiliary = population;
        
        // Embaralha o vetor
        std::shuffle(auxiliary.begin(), auxiliary.end(), gen);
    
        Solution* first = GeneticAlgorithm::findMinimal(auxiliary);
        
        Solution* second = GeneticAlgorithm::findMinimal(auxiliary);
        
    
        selectedPairs.push_back(std::make_pair(first, second));
    }
    return selectedPairs;
}

// Crossover
std::vector<Solution*> GeneticAlgorithm::onePointCrossover(std::vector<std::pair<Solution*, Solution*>> pop) {

    std::vector<Solution*> result;

    for(int i = 0; i < pop.size(); i++){

        std::pair<Solution*, Solution*> pair = pop[i];

        Solution* dad = pair.first;
        Solution* mom = pair.second;
        
        int solutionLength = this->g->numNodes;
        int randomIndex = dis(gen);
        
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

        if(this->populationSize % 2 == 1 && i+1 == pop.size()){
            Solution* f = new Solution(firstChild, this->prd);
            Solution* s = new Solution(secondChild, this->prd);

            if(f->fitness < s->fitness){
                result.push_back(f);
                delete s;
            }else{
                result.push_back(s);
                delete f;
            }
        }else{
            result.push_back(new Solution(firstChild, this->prd));
            result.push_back(new Solution(secondChild, this->prd));
        }
    }
    return result;
}


// ok atilio
// Mutation
void GeneticAlgorithm::randomMutation(std::vector<Solution*>& pop){
    for (Solution*& sol : pop) {
        bool changed = false;
        // For every gen, look if have mutation
        for (int k = 0; k < sol->solution.size(); k++) {
            if (dis(gen) < this->mutationRate) {
                sol->solution[k] = disInt(gen) * 2;
                changed = true;
            }
        }
        // Solution changed, recalculation required.
        if (changed) {
            sol = this->changeSolution(sol);
        }
    }
}

//Elitism
std::vector<Solution*> GeneticAlgorithm::defaultElitism(std::vector<Solution*>& current, std::vector<Solution*>& newPop) {

    std::vector<Solution*> result;
    
    // Sort the solution vectors
    std::sort(current.begin(), current.end(), [](Solution* a, Solution* b) { return *a < *b;}); 
    std::sort(newPop.begin(), newPop.end(), [](Solution* a, Solution* b) { return *a < *b;});

    for(int i = 0; i < current.size(); i++){ 
            if(i < this->elitismSize){
                result.push_back(current[i]); 
            }else{
                delete current[i];
            }
        }
    
    // Copy the best features from this generagtion and free up the rest of memory.
    int res = newPop.size() - this->elitismSize;
    
    for(int i = 0; i < newPop.size(); i++){ 
        if(i < res){
            result.push_back(newPop[i]);
        }else {
            delete newPop[i];
        }  
    } 
    // Sort the result to get best solution on first position
    std::sort(result.begin(), result.end(), [](Solution* a, Solution* b) { return *a < *b;}); 
    return result;
}

// Auxiliary functions

// ok atilio
Solution* GeneticAlgorithm::findMinimal(std::vector<Solution*>& preSelected) {
    Solution* minimal = preSelected[0];
    int indexToRemove = 0;
    for(int i = 1; i < this->tournamentSize; i++){
        if(*preSelected[i] < *minimal){
            minimal = preSelected[i];
            indexToRemove = i;
        }
    }
    preSelected.erase(preSelected.begin() + indexToRemove);
    return minimal;
}

// ok atilio
Solution* GeneticAlgorithm::changeSolution(Solution* element){
    Solution* aux = new Solution(element->solution, this->prd);
    delete element;
    return aux;
}


// ok atilio
std::vector<Solution*> GeneticAlgorithm::initializePopulation(){

    // Call randomized initialization to get a half of population
    std::vector<Solution*> aux = this->prd->randomizedInitialization(this->populationSize - 1);

    // Call greedy initialization to get +1 solution, probabily the best solution in this population
    aux.push_back(this->prd->greedyInitialization());
    
    // Rest of population will be generated for randomizedSolution function
    for(int i = aux.size(); i < this->populationSize; i++){
       aux.push_back(this->prd->randomSolution());
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
        std::cout << "Fitness: " << ptr->fitness << std::endl;
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
    std::cout << "Fitness: " << ptr->fitness << std::endl;
    std::cout << "Solution: ";
    for(int i = 0; i < ptr->solution.size(); i++){
        std::cout << ptr->solution[i] << " ";

        if(i == ptr->solution.size() - 1){
            std::cout << std::endl << std::endl;
        }
    }

}

