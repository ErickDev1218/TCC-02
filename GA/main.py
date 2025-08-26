from Graph import Graph


def main():

    Nodes = ['A', 'B', 'C', 'D', 'E',]
    Edges = [('A', 'B'), ('A', 'C'), ('B', 'D'), ('C', 'D'), ('D', 'E')]

    g = Graph(Nodes, Edges)
    
    g.print_graph()
    g.label_nodes()
    labels = g.get_labels()
    print("Node Labels:", labels)




if __name__ == "__main__":
    main()