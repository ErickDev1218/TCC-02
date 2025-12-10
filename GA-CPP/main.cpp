#include "GA.hpp"
#include <fstream>
#include <filesystem>

#define IRACE 1

namespace fs = std::filesystem;
using namespace std;

struct Parameters {

    #if IRACE
    long unsigned iraceSeed = 0;
    #endif

    int maxStagnant = 350;
    int generations = 500;
    int tournamentSize = 5;
    int trials = 1;
    float crossoverRate = 0.5;
    float populationFactor = 3;
    float elitismRate = 0.1;
    float mutationRate = 0.07;
    std::string file_path;
    std::string output_file = "results.csv";
};

// struct Result {
//     std::string graph_name;
//     int node_count;
//     int edge_count;
//     float graph_density;
//     int fitness;
//     double elapsed_time;
// };

Parameters parse_args(int argc, char *argv[]);
void printParameters(const Parameters& p);
void ensure_csv_header(const std::string &filename);
void write_result_to_csv(const std::string &filename, const Result &result);
void runGA(Parameters params, const std::string& path);

int main(int argc, char *argv[]){
    Parameters params = parse_args(argc, argv);

    #if !IRACE
    ensure_csv_header(params.output_file);
    #endif

    runGA(params, params.file_path);
    
    return 0;
}

void runGA(Parameters params, const std::string& path) {

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
    
    Graph* g = new Graph(num_vertex, num_edges, edges, fs::path (path).stem().string());
    
    file.close();
    
    for(int trial = 0; trial < params.trials; trial++){
        
        
        GeneticAlgorithm* GA = new GeneticAlgorithm(g, params.populationFactor, params.tournamentSize, params.maxStagnant, params.mutationRate, params.elitismRate, params.generations);
    
        // auto begin = std::chrono::high_resolution_clock::now();
    
        Result res = GA->gaFlow();
    
        // auto end = std::chrono::high_resolution_clock::now();
        // auto elapsed_time = std::chrono::duration<double>(end - begin);
    
        // res.node_count = num_vertex;
        // res.edge_count = num_edges;
        // res.elapsed_time = elapsed_time.count();
        // res.graph_density = static_cast<float>(2 * num_edges) / (num_vertex * (num_vertex - 1));
        // res.graph_name = GA->g->graphName;
        // res.fitness = GA->bestFitness;

        #if !IRACE
        write_result_to_csv(params.output_file, res);
        #endif

        #if IRACE
        cout << res.fitness << endl;
        #endif

        delete GA;
    }

    delete g;

}

void ensure_csv_header(const std::string &filename) {
    bool file_exists = std::filesystem::exists(filename);
    std::ofstream file;
    if (!file_exists) {
        file.open(filename);
        file << "graph_name,graph_order,graph_size,density,fitness_value,elapsed_time(seconds)\n";
        file.close();
    } 
}

void write_result_to_csv(const std::string &filename, const Result &result) {
    std::ofstream file(filename, std::ios::app);
    file << result.graph_name << "," << result.node_count << ","
         << result.edge_count << "," << result.graph_density << "," << result.fitness << ","
         << result.elapsed_time << "\n"; 
    file.close();
}


void printParameters(const Parameters& p) {
    std::cout << "===== Genetic Algorithm Parameters =====\n";
    std::cout << std::left;

    std::cout << std::setw(20) << "Graph file:"       << p.file_path       << "\n";
    std::cout << std::setw(20) << "Output file:"      << p.output_file     << "\n";
    std::cout << std::setw(20) << "Generations:"      << p.generations     << "\n";
    std::cout << std::setw(20) << "Max stagnant:"     << p.maxStagnant     << "\n";
    std::cout << std::setw(20) << "Tournament size:"  << p.tournamentSize  << "\n";
    std::cout << std::setw(20) << "Population factor:"<< p.populationFactor<< "\n";
    std::cout << std::setw(20) << "Elitism rate:"     << p.elitismRate     << "\n";
    std::cout << std::setw(20) << "Mutation rate:"    << p.mutationRate    << "\n";
    std::cout << std::setw(20) << "Total trials:"    << p.trials    << "\n";
    std::cout << "=========================================\n";
}

Parameters parse_args(int argc, char *argv[]) {
    Parameters parameters;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> [options]\n"
                  << "Options:\n"
                  << "  --crossover VALUE\n"
                  << "  --stagnation VALUE\n"
                  << "  --generations VALUE\n"
                  << "  --populationFactor VALUE\n"
                  << "  --tournament VALUE\n"
                  << "  --elitism VALUE\n"
                  << "  --mutation VALUE\n"
                  << "  --trials VALUE\n"
                  << "  --output FILE\n";
        exit(1);
      }

    #if !IRACE
    parameters.file_path = argv[1];

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--stagnation" && i + 1 < argc) {
            parameters.maxStagnant = std::stoi(argv[++i]);

        } else if (arg == "--generations" && i + 1 < argc) {
            parameters.generations = std::stoi(argv[++i]);

        } else if (arg == "--populationFactor" && i + 1 < argc) {
            parameters.populationFactor = std::stoi(argv[++i]);

        } else if (arg == "--tournament" && i + 1 < argc) {
            parameters.tournamentSize = std::stoi(argv[++i]);

        } else if (arg == "--elitism" && i + 1 < argc) {
            parameters.elitismRate = std::stof(argv[++i]);

        } else if (arg == "--mutation" && i + 1 < argc) {
            parameters.mutationRate = std::stof(argv[++i]);

        } else if (arg == "--output" && i + 1 < argc) {
            parameters.output_file = argv[++i];

        } else if (arg == "--trials" && i + 1 < argc) {
            parameters.trials = std::stoi(argv[++i]);

        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exit(1);
        }
    }

    #endif

    #if IRACE

    parameters.iraceSeed = std::stoul(argv[3]);         // seed for random number generator 
    parameters.file_path = argv[4];                     // path for instance
    
    for (int i = 5; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--stagnation" && i + 1 < argc) {
            parameters.maxStagnant = std::stoi(argv[++i]);

        } else if (arg == "--generations" && i + 1 < argc) {
            parameters.generations = std::stoi(argv[++i]);

        } else if (arg == "--populationFactor" && i + 1 < argc) {
            parameters.populationFactor = std::stoi(argv[++i]);

        } else if (arg == "--tournament" && i + 1 < argc) {
            parameters.tournamentSize = std::stoi(argv[++i]);

        } else if (arg == "--elitism" && i + 1 < argc) {
            parameters.elitismRate = std::stof(argv[++i]);

        } else if (arg == "--mutation" && i + 1 < argc) {
            parameters.mutationRate = std::stof(argv[++i]);

        } else if (arg == "--output" && i + 1 < argc) {
            parameters.output_file = argv[++i];

        } else if (arg == "--trials" && i + 1 < argc) {
            parameters.trials = std::stoi(argv[++i]);

        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exit(1);
        }
    }

    #endif

    return parameters;
}