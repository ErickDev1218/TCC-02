import random
from Graph import Graph

class PRD:

    def __init__(self, graph):
        self.graph = graph
        self.start = [False for _ in graph.nodes]
    

    def randomized_initialization(self, many_solutions = None):

        g = self.graph
        num_vertices = len(g.nodes)
        indices = list(range(num_vertices))
        random.shuffle(indices)

        solutions = []

        for i, start_idx in enumerate(indices):

            # Para se many_solutions for fornecido, pare após atingir esse número
            if many_solutions is not None and i >= many_solutions:
                break

            # Reinicializa labels e dominância
            for node in g.nodes:
                node.label = -1
                node.dominated = False

            v = g.nodes[start_idx]
            if v.label == -1 and not v.dominated:

                # Verifica se o vértice é isolado
                if v.edges == []:
                    v.label = 1
                    v.dominated = True
                    continue

                v.label = 2
                v.dominated = True
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

            # Marca como dominado todo vértice com label 0 que tenha vizinho com label 2
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
                if (v.label == -1 or v.label == 0) and not v.dominated:
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

            # Salva a solução atual (cópia dos labels)
            solution = [node.label for node in g.nodes]
            solutions.append(solution)

        return solutions
    

    def degree_based_initialization(self):
        g = self.graph
        for node in g.nodes:
            node.label = -1
            node.dominated = False

        while True:
            unlabelled = [node for node in g.nodes if node.label == -1]
            if not unlabelled:
                break

            # Pega o vértice de maior grau
            v = max(unlabelled, key=lambda n: len(n.edges))

            # Trata vértice isolado
            if len(v.edges) == 0:
                v.label = 1
                v.dominated = True
                continue

            neighbors = []
            can_be_2 = True

            for edge in v.edges:
                neighbor_value = edge[1] if edge[0] == v.value else edge[0]
                neighbor = next((n for n in g.nodes if n.value == neighbor_value), None)
                if neighbor:
                    neighbors.append(neighbor)
                    # Se algum vizinho já foi dominado por um 2, não pode ser 2
                    if neighbor.label == 0 and neighbor.dominated:
                        can_be_2 = False
                    # Se algum vizinho já é 2, não pode ser 2
                    # Talvez isso não seja necessário, mas é uma verificação extra
                    if neighbor.label == 2:
                        can_be_2 = False
                    
                    if neighbor.label == 1:
                        can_be_2 = False

            if can_be_2 and not v.dominated:
                v.label = 2
                v.dominated = True
                for neighbor in neighbors:
                    if neighbor.label == -1:
                        neighbor.label = 0
                        neighbor.dominated = True
            elif not v.dominated:
                v.label = 1
                v.dominated = True
            else:
                v.label = 0 if v.label == -1 else v.label

        return [node.label for node in g.nodes]
    

    def check_prd(self, solution):
        g = Graph([node.value for node in self.graph.nodes], self.graph.edges)
        g.label_nodes(solution)
        for v in g.nodes:
            if v.label == 0:
                count = 0
                for edge in v.edges:
                    neighbor_value = edge[1] if edge[0] == v.value else edge[0]
                    neighbor_node = next((n for n in g.nodes if n.value == neighbor_value), None)
                    if neighbor_node and neighbor_node.label == 2:
                        count += 1
                if count != 1:
                    return False
            
        return True