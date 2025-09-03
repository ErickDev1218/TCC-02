from Graph import Graph
from PRD import PRD
from GA import GA
def main():

    Nodes = [0,1,2,3,4,5,6]
    Edges = [(0, 4), (0, 2), (1, 2), (2, 3), (5,1), (5,4), (6,4)]

    g = Graph(Nodes, Edges)

    ga = GA(population_size=len(Nodes), mutation_rate=0.1, crossover_rate=0.7, generations=5, graph=g)
    ga.initialize_population()
    
    # prd = PRD(g)
    # degree_labels = prd.degree_based_initialization()
    # res = prd.randomized_initialization(7)
    # for sol in res:
    #     print(prd.check_prd(sol))
    # for i in range(0,5):
    #     print(f"\n--- Iteration {i+1} ---")
    #     g.reset_labels_and_dominance()
    #     res = prd.randomized_initialization()
    #     if res:
    #         labels = g.get_labels()
    #         print("Node Labels after Roman Perfect Domination:", labels)
    #         dominates = g.get_dominated_status()
    #         print("Node Dominated Status:", dominates)

    # res = prd.randomized_initialization(7)
    # res = prd.degree_based_initialization()

    # if res:
        # labels = g.get_labels()
        # print("Node Labels after Roman Perfect Domination:", labels)
        # dominates = g.get_dominated_status()
        # print("Node Dominated Status:", dominates)
        # print(res)
        # print(f"Resolução é uma PRD: {prd.check_prd(prd)}")
    # g.print_graph()




if __name__ == "__main__":
    main()