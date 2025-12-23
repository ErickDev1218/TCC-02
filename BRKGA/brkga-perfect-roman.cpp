#include <iostream>
#include <algorithm>
#include <string>
#include <filesystem>
#include <chrono>
#include <limits>
#include <cmath>
#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "DecoderRoman.h"
#include "Graph.h"

#define DEBUG 0
#define IRACE 0

struct AlgorithmParameters {
	std::string file_path;
	std::string output_file = "results.csv";

	unsigned n = 10;		    // size of chromosomes
	unsigned population_factor = 100;		    // size of population
	double pe = 0.20;		    // fraction of population to be the elite-set
	double pm = 0.10;		    // fraction of population to be replaced by mutants
	double rhoe = 0.70;	        // probability that offspring inherit an allele from elite parent
	unsigned K = 2;		        // number of independent populations
	unsigned MAXT = 2;	        // number of threads for parallel decoding
	unsigned X_INTVL = 100;	    // exchange best individuals at every 100 generations
	unsigned X_NUMBER = 2;	    // exchange top 2 best
	unsigned MAX_GENS = 1000;	// maximum number of generations
	unsigned MAX_STAGT = 400;   // number of stagnation
	unsigned trials = 1;        // number of executions of the genetic algorithm
};

struct Result {
    std::string graph_name;
    size_t node_count;
    size_t edge_count;
    float graph_density;
    int fitness;
    double elapsed_time;
};

AlgorithmParameters parse_args(int argc, char *argv[]);
void ensure_csv_header(const std::string &filename);
void write_result_to_csv(const std::string &filename, const Result &result);
bool isFeasible(const std::vector<double>& chromosome, const Graph& graph);


int main(int argc, char *argv[]) {
    
	AlgorithmParameters parameters = parse_args(argc, argv);

	ensure_csv_header(parameters.output_file);

	std::string filename(parameters.file_path);


	Graph g(filename);

	parameters.n = g.getOrder();

	for (size_t trial = 0; trial < parameters.trials; ++trial) {
		#if DEBUG
        std::cout << "------------------------------------------------------------\n";
        std::cout << "Running trial " << (trial+1) << " of " << parameters.trials << "\n";
        #endif

		// initialize the decoder
		DecoderRoman decoder(g);

		const long unsigned rngSeed = trial;	// seed to the random number generator
		MTRand rng((rngSeed + 1) * 1234);	    // initialize the random number generator

        unsigned pop_size = parameters.n / parameters.population_factor;
        
		// initialize the BRKGA-based heuristic
		BRKGA<DecoderRoman, MTRand> algorithm(parameters.n, pop_size, parameters.pe, 
			parameters.pm, parameters.rhoe, decoder, rng, parameters.K, parameters.MAXT);

		#if DEBUG
        std::cout << "Population size = " << pop_size << std::endl;
		std::cout << "Running for " << parameters.MAX_GENS << " generations..." << std::endl;
		#endif

		auto begin = std::chrono::high_resolution_clock::now();
		
		unsigned generation = 0;		// current generation
		unsigned stagnant_count = 0;

		double bestFitness = std::numeric_limits<double>::max();
		
		do {
			algorithm.evolve();	// evolve the population for one generation
			
			if(bestFitness > algorithm.getBestFitness()) {
				bestFitness = algorithm.getBestFitness();
				stagnant_count = 0;
				#if DEBUG
				std::cout << "generation " << generation << ", new best fitness: " << bestFitness << "\n";
				#endif
			} else {
				stagnant_count++;
			}
			
			if((++generation) % parameters.X_INTVL == 0) {
				algorithm.exchangeElite(parameters.X_NUMBER);	// exchange top individuals
			}
		} while (generation < parameters.MAX_GENS && stagnant_count < parameters.MAX_STAGT);

		auto end = std::chrono::high_resolution_clock::now();
    	//auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end-begin);
        auto elapsed_time = std::chrono::duration<double>(end-begin);

		std::filesystem::path input_path(parameters.file_path);
		Result result;
		result.graph_name = input_path.filename().string();
		result.node_count = g.getOrder();
		result.edge_count = g.getSize();
		result.fitness = algorithm.getBestFitness();
		result.elapsed_time = elapsed_time.count();
		result.graph_density = g.getDensity();

		#if DEBUG
        std::cout << "\nResult of the trial " << trial << ":\n";
         std::cout << "  graph path: " << parameters.file_path << std::endl;
        std::cout << "  graph name: " << result.graph_name << std::endl;
        std::cout << "  number of vertices: " << result.node_count << std::endl;
        std::cout << "  number of edges: " << result.edge_count << std::endl;
        std::cout << "  graph density: " << result.graph_density << std::endl;
        std::cout << "  fitness: " << result.fitness << std::endl;
        std::cout << "  time (seconds): " << result.elapsed_time << std::endl;
        std::cout << std::endl;
        #endif

		#if IRACE
		std::cout << algorithm.getBestFitness();
		#endif

        #if !IRACE
		write_result_to_csv(parameters.output_file, result);
        #endif
	}

	#if DEBUG
    std::cout << "\nTodos os resultados foram salvos em: " << parameters.output_file << std::endl;
    #endif

	return 0;
}


AlgorithmParameters parse_args(int argc, char *argv[]) {
    AlgorithmParameters parameters;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <graph_file> [options]\n"
                  << "Options:\n"
                  << "  --population_factor VALUE\n"
                  << "  --pe VALUE\n"
                  << "  --pm VALUE\n"
                  << "  --rhoe VALUE\n"
                  << "  --K VALUE\n"
                  << "  --MAXT VALUE\n"
                  << "  --X_INTVL VALUE\n"
				  << "  --X_NUMBER VALUE\n"
				  << "  --MAX_GENS VALUE\n"
				  << "  --MAX_STAGT VALUE\n"
    
                  << "  --trials VALUE\n"
                  << "  --output FILE\n";
        exit(1);
      }

    #if !IRACE
    parameters.file_path = argv[1];

    for (int i{2}; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--population_factor" && i + 1 < argc) {
            parameters.population_factor = std::stoul(argv[++i]);
        } else if (arg == "--pe" && i + 1 < argc) {
            parameters.pe = std::stod(argv[++i]);
        } else if (arg == "--pm" && i + 1 < argc) {
            parameters.pm = std::stod(argv[++i]);
        } else if (arg == "--rhoe" && i + 1 < argc) {
            parameters.rhoe = std::stod(argv[++i]);
        } else if (arg == "--K" && i + 1 < argc) {
            parameters.K = std::stoul(argv[++i]);
        } else if (arg == "--MAXT" && i + 1 < argc) {
            parameters.MAXT = std::stoul(argv[++i]);
        } else if (arg == "--X_INTVL" && i + 1 < argc) {
            parameters.X_INTVL = std::stoul(argv[++i]);
        } else if (arg == "--X_NUMBER" && i + 1 < argc) {
            parameters.X_NUMBER = std::stoul(argv[++i]);
        } else if (arg == "--MAX_GENS" && i + 1 < argc) {
            parameters.MAX_GENS = std::stoul(argv[++i]);
        } else if (arg == "--MAX_STAGT" && i + 1 < argc) {
            parameters.MAX_STAGT = std::stoul(argv[++i]);
        } else if (arg == "--trials" && i + 1 < argc) {
            parameters.trials = std::stoul(argv[++i]);
        }
        else if (arg == "--input" && i + 1 < argc){
            parameters.file_path = argv[++i];
        }else if (arg == "--output" && i + 1 < argc) {
            parameters.output_file = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exit(1);
        }
    }

    #endif

    #if IRACE
    parameters.file_path = argv[4];         

    for (int i{5}; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--population_factor" && i + 1 < argc) {
            parameters.population_factor = std::stoul(argv[++i]);
        } else if (arg == "--pe" && i + 1 < argc) {
            parameters.pe = std::stod(argv[++i]);
        } else if (arg == "--pm" && i + 1 < argc) {
            parameters.pm = std::stod(argv[++i]);
        } else if (arg == "--rhoe" && i + 1 < argc) {
            parameters.rhoe = std::stod(argv[++i]);
        } else if (arg == "--K" && i + 1 < argc) {
            parameters.K = std::stoul(argv[++i]);
        } else if (arg == "--MAXT" && i + 1 < argc) {
            parameters.MAXT = std::stoul(argv[++i]);
        } else if (arg == "--X_INTVL" && i + 1 < argc) {
            parameters.X_INTVL = std::stoul(argv[++i]);
        } else if (arg == "--X_NUMBER" && i + 1 < argc) {
            parameters.X_NUMBER = std::stoul(argv[++i]);
        } else if (arg == "--MAX_GENS" && i + 1 < argc) {
            parameters.MAX_GENS = std::stoul(argv[++i]);
        } else if (arg == "--MAX_STAGT" && i + 1 < argc) {
            parameters.MAX_STAGT = std::stoul(argv[++i]);
        } else if (arg == "--trials" && i + 1 < argc) {
            parameters.trials = std::stoul(argv[++i]);
        }
        else if (arg == "--input" && i + 1 < argc){
            parameters.file_path = argv[++i];
        }else if (arg == "--output" && i + 1 < argc) {
            parameters.output_file = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            exit(1);
        }
    }
    #endif

    return parameters;
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
