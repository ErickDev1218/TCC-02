from Graph import Graph
from PRD import PRD
from GA import GA
def main():

    Nodes = [0,1,2,3,4,5,6]

    Edges = [(0, 4), (0, 2), (1, 2), (2, 3), (5,1), (5,4),(6,2),(6,3)]

    g = Graph(Nodes, Edges)
    ga = GA(population_size=len(Nodes), mutation_rate=0.5, eletism_size = int(len(Nodes) * 0.2), crossover_rate=0.7, generations=10, graph=g, config = ["roullette", "one_point", "shift", "partition"])
    ga.initialize_population()
    ga.ga_flow()


if __name__ == "__main__":
    main()