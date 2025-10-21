#include "Solution.hpp"
#include "Graph.hpp"
#include "GA.hpp"
#include "Solution.hpp"
#include "PRD.hpp"
#include <vector>
#include <iostream>
#include <utility>
#include <iomanip> // para std::setw, opcional
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
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

Graph* readGraphFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo: " + path);
    }
    int qtd_vertices, qtd_arestas;

    file >> qtd_vertices >> qtd_arestas;
    
    vector<pair<int, int>> edges;
    // edges.resize(qtd_arestas);
    int u, v;
    while (file >> u >> v) {
        edges.push_back(make_pair(u,v)); // Classe grafo é capaz de criar a aresta v u
    }


    Graph* g = new Graph(qtd_vertices, edges);

    file.close();
    return g;
}

vector<Graph*> readAllGraphsFromDir(const std::string& dirPath) {
    vector<Graph*> graphs;

    // std::cout << "Tentando abrir diretório: " << std::filesystem::absolute(dirPath) << std::endl;
    // if (!std::filesystem::exists(dirPath)) {
    //     std::cerr << "❌ Diretório não encontrado!" << std::endl;
    //     exit(1);
    // }
    
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".txt") {
            std::cout << "📄 Lendo: " << entry.path().filename() << std::endl;
            try {
                graphs.push_back(readGraphFromFile(entry.path().string()));
            } catch (const std::exception& e) {
                std::cerr << "Erro: " << e.what() << std::endl;
            }
        }
    }

    std::cout << "\n✅ Total de grafos carregados: " << graphs.size() << std::endl;
    return graphs;
}

int main(int argc, char *argv[]){

    Parameters params = parse_args(argc, argv);
    printParameters(params);    

    vector<Graph*> graphs = readAllGraphsFromDir(params.file_path);

    // for(int i = 0; i < graphs.size(); i++){
    //     delete graphs[i];
    // }
    GeneticAlgorithm* GA = new GeneticAlgorithm(graphs[4], params.populationFactor, params.tournamentSize, params.maxStagnant, params.mutationRate, params.elitismRate, params.crossoverRate, params.generations);
    GA->gaFlow();


    return 0;
}