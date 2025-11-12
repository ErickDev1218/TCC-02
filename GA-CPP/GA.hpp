#ifndef GA_HPP
#define GA_HPP
#include "Graph.hpp"
#include "Solution.hpp"
#include "PRD.hpp"

class GeneticAlgorithm {
        public:
            float mutationRate;
            float elitismSize;
            float crossoverRate;
            int populationSize;
            int maxGenerations;
            int maxStagnant;
            int tournamentSize;

            std::mt19937 gen;
            std::uniform_real_distribution<> dis; // [0, 1]
            std::uniform_int_distribution<> disInt;   // {0, 1}

            std::vector<Solution*> population;
            Graph* g = nullptr;
            PRD* prd = nullptr;
            int bestFitness = std::numeric_limits<int>::max();

            GeneticAlgorithm(Graph* g, int popFactor, int tournSize, int stagnant,float mutRate, float eleSize, float crosRate, int maxGenerations);

            ~GeneticAlgorithm();

            void gaFlow();


            // OPERATORS

            // Selection
            std::vector<std::pair<Solution*, Solution*>> tournamentSelection(std::vector<Solution*> population);

            // Crossover
            std::vector<Solution*> onePointCrossover(std::vector<std::pair<Solution*, Solution*>> pop);
            
            // Mutation
            void randomMutation(std::vector<Solution*>& pop);

            // Elitism
            std::vector<Solution*> defaultElitism(std::vector<Solution*>& current, std::vector<Solution*>& newPop);

        private:

            // Auxiliary functions
            Solution* findMinimal(std::vector<Solution*>& preSelected);
            Solution* changeSolution(Solution* element);
            std::vector<Solution*> initializePopulation();
            void printVectorGA(std::vector<int> x, std::vector<int> y);
            void printSolutions(std::vector<Solution*>& pop);
            void printSingleSolution(Solution* solution);

};


#endif
