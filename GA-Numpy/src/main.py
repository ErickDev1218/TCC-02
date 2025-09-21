from Graph import Graph
from GA import GA
from PRD import PRD 
import numpy as np

def main():
    
    Nodes = np.array([0, 1, 2, 3, 4, 5, 6])
    Edges = np.array([(0, 4), (0, 2), (1, 2), (2, 3), (5, 1), (5, 4), (6, 2), (6, 3)])

    g = Graph(Nodes, Edges)
    prd = PRD(g)
    sol = prd.randomized_initialization()
    for s in sol:
        print('solution:', s)
        print('PRD check', prd.check_prd_v2(s))
    # ga = GA(population_size=len(Nodes), mutation_rate=0.5, elitism_size=int(len(Nodes) * 0.2), crossover_rate=0.7, generations=10, graph=g, config=["roulette", "one_point", "shift", "partition"])
    # ga.initialize_population()
    # ga.ga_flow()

if __name__ == "__main__":
    main()