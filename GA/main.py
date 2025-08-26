from Graph import Graph
from PRD import PRD

def main():

    Nodes = [0,1,2,3,4]
    Edges = [(0, 4), (0, 2), (1, 2), (2, 3)]

    g = Graph(Nodes, Edges)

    PRD(g)
    labels = g.get_labels()
    print("Node Labels after Roman Perfect Domination:", labels)
    dominates = g.get_dominated_status()
    print("Node Dominated Status:", dominates)




if __name__ == "__main__":
    main()