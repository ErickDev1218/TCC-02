import random

class PRD:

    def __init__(self, graph):
        self.graph = graph
        self.randomized_initialization(graph)

    def randomized_initialization(self, g):
        # Pegue um número aleatório entre 0 e o número de vértices
        num_vertices = len(g.nodes)
        random_index = random.randint(0, num_vertices - 1)  
        print('Random Index:', random_index)
        while True:
            progress = False
            # for v in g.nodes:
            if g.nodes[random_index] == -1 and not v.dominated:
                v.label = 2
                v.dominated = True
                progress = True
                for edge in v.edges:
                    u = edge[1] if edge[0] == v.value else edge[0]
                    neighbor = next((n for n in g.nodes if n.value == u), None)
                    if neighbor and neighbor.label == -1 and not neighbor.dominated:
                        neighbor.label = 0
                        neighbor.dominated = True
                        for e in neighbor.edges:
                            w = e[1] if e[0] == neighbor.value else e[0]
                            neighbor2 = next((n for n in g.nodes if n.value == w), None)
                            if neighbor2 and neighbor2.label == -1 and not neighbor2.dominated:
                                neighbor2.label = 0
                break  # Reinicia o loop para pegar o novo estado dos nós
            if not progress:
                break
        # Ao final, marque como dominado todo vértice com label 0 que tenha vizinho com label 2
        for v in g.nodes:
            if v.label == 0 and not v.dominated:
                for edge in v.edges:
                    neighbor_value = edge[1] if edge[0] == v.value else edge[0]
                    neighbor_node = next((n for n in g.nodes if n.value == neighbor_value), None)
                    if neighbor_node and neighbor_node.label == 2:
                        v.dominated = True
                        break

        # Trata vértices restantes com label -1 e não dominados
        for v in g.nodes:
            if v.label == -1 or v.label == 0 and not v.dominated:
                has_label_2_neighbor = False
                for edge in v.edges:
                    neighbor_value = edge[1] if edge[0] == v.value else edge[0]
                    neighbor_node = next((n for n in g.nodes if n.value == neighbor_value), None)
                    if neighbor_node and neighbor_node.label == 2:
                        has_label_2_neighbor = True
                        break
                if not has_label_2_neighbor:
                    v.label = 1
                    v.dominated = True