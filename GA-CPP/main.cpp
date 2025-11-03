#include "GA.hpp"
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
    std::cout << std::left;

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

void readGraphFromFile(Parameters params, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + path);
    }
    int num_vertex, num_edges;

    file >> num_vertex >> num_edges;
    
    vector<pair<int, int>> edges;
    edges.reserve(num_edges);
    int u, v;
    while (file >> u >> v) {
        edges.push_back(make_pair(u,v));
    }

    Graph* g = new Graph(num_vertex, edges, fs::path (path).stem().string());

    file.close();

    GeneticAlgorithm* GA = new GeneticAlgorithm(g, params.populationFactor, params.tournamentSize, params.maxStagnant, params.mutationRate, params.elitismRate, params.crossoverRate, params.generations);
    
    GA->gaFlow();

    delete GA;
}

void readAllGraphsFromDir(Parameters params) {
    const std::string& dirPath = params.file_path;
    
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".txt") {
            std::cout << "📄 Reading: " << entry.path().filename() << std::endl;
            try {
                readGraphFromFile(params, entry.path().string());
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[]){

    Parameters params = parse_args(argc, argv);
    printParameters(params);    
    readAllGraphsFromDir(params);

    return 0;
}