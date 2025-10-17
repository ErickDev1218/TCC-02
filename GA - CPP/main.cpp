#include "Solution.hpp"
#include "Graph.hpp"
#include "GA.hpp"
#include "Solution.hpp"
#include "PRD.hpp"
#include <vector>
#include <iostream>
#include <utility>
#include <iomanip> // para std::setw, opcional
using namespace std;



struct Parameters {
    int maxStagnant = 350;
    int generations = 500;
    int tournamentSize = 5;
    float crossoverRate = 0.5;
    float populationFactor = 3;
    float elitismRate = 0.1;
    float mutationRate = 0.07;
    std::string file_path;
    std::string output_file = "results.csv";
};

Parameters parse_args(int argc, char *argv[]) {
    Parameters parameters;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> [options]\n"
                  << "Options:\n"
                  << "  --crossover VALUE\n"
                  << "  --stagnation VALUE\n"
                  << "  --generations VALUE\n"
                  << "  --population VALUE\n"
                  << "  --tournament VALUE\n"
                  << "  --elitism VALUE\n"
                  << "  --mutation VALUE\n"
    
                  << "  --output FILE\n";
        exit(1);
      }

    parameters.file_path = argv[1];

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--crossover" && i + 1 < argc) {
            parameters.crossoverRate = std::stof(argv[++i]);

        } else if (arg == "--stagnation" && i + 1 < argc) {
            parameters.maxStagnant = std::stoi(argv[++i]);

        } else if (arg == "--generations" && i + 1 < argc) {
            parameters.generations = std::stoi(argv[++i]);

        } else if (arg == "--population" && i + 1 < argc) {
            parameters.populationFactor = std::stoi(argv[++i]);

        } else if (arg == "--tournament" && i + 1 < argc) {
            parameters.tournamentSize = std::stoi(argv[++i]);

        } else if (arg == "--elitism" && i + 1 < argc) {
            parameters.elitismRate = std::stof(argv[++i]);

        } else if (arg == "--mutation" && i + 1 < argc) {
            parameters.mutationRate = std::stof(argv[++i]);

        } else if (arg == "--output" && i + 1 < argc) {
            parameters.output_file = argv[++i];

        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exit(1);
        }
    }

    return parameters;
}

void printParameters(const Parameters& p) {
    std::cout << "===== Genetic Algorithm Parameters =====\n";
    std::cout << std::left; // alinha à esquerda

    std::cout << std::setw(20) << "Graph file:"       << p.file_path       << "\n";
    std::cout << std::setw(20) << "Output file:"      << p.output_file     << "\n";
    std::cout << std::setw(20) << "Generations:"      << p.generations     << "\n";
    std::cout << std::setw(20) << "Max stagnant:"     << p.maxStagnant     << "\n";
    std::cout << std::setw(20) << "Tournament size:"  << p.tournamentSize  << "\n";
    std::cout << std::setw(20) << "Crossover rate:"   << p.crossoverRate   << "\n";
    std::cout << std::setw(20) << "Population factor:"<< p.populationFactor<< "\n";
    std::cout << std::setw(20) << "Elitism rate:"     << p.elitismRate     << "\n";
    std::cout << std::setw(20) << "Mutation rate:"    << p.mutationRate    << "\n";
    std::cout << "=========================================\n";
}

int main(int argc, char *argv[]){

    Parameters params = parse_args(argc, argv);
    printParameters(params);
    int v = 7;
    vector<pair<int, int>> edges;

    edges.push_back(make_pair(0, 4));
    edges.push_back(make_pair(0, 2));
    edges.push_back(make_pair(1, 2));
    edges.push_back(make_pair(2, 3));
    edges.push_back(make_pair(5, 1));
    edges.push_back(make_pair(5, 4));
    edges.push_back(make_pair(6, 2));
    edges.push_back(make_pair(6, 3));

    Graph* graph = new Graph(v,edges);
    std::cout << "AQUI" << endl;

    GeneticAlgorithm* GA = new GeneticAlgorithm(graph, params.populationFactor, params.tournamentSize, params.maxStagnant, params.mutationRate, params.elitismRate, params.crossoverRate, params.generations);

    GA->gaFlow();

    return 0;
} 