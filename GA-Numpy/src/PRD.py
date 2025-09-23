import numpy as np

class PRD:
    def __init__(self, graph):
        self.graph = graph

    def randomized_initialization(self, many_solutions=None):
        g = self.graph
        num_vertices = len(g.nodes)
        indices = np.arange(num_vertices)
        np.random.shuffle(indices)
        adjacency = g.adjacency_matrix

        

        solutions = []

        for i, start_idx in enumerate(indices):
            # Para se many_solutions for fornecido, pare após atingir esse número
            if many_solutions is not None and i >= many_solutions:
                break

            labels = np.full(num_vertices, -1, dtype=int)
            dominated = np.zeros(num_vertices, dtype=bool)

            # Verifica se o vértice é isolado
            if not adjacency[start_idx].any():
                labels[start_idx] = 1
                dominated[start_idx] = True
                continue
            else:
                labels[start_idx] = 2
                dominated[start_idx] = True

                # Vizinhos diretos
                neighbors = np.where(adjacency[start_idx] == 1)[0]
                for neighbor in neighbors:
                    if labels[neighbor] == -1 and not dominated[neighbor]:
                        labels[neighbor] = 0
                        dominated[neighbor] = True

                        # Vizinhos dos vizinhos
                        neighbors2 = np.where(adjacency[neighbor] == 1)[0]
                        for neighbor2 in neighbors2:
                            if labels[neighbor2] == -1 and not dominated[neighbor2]:
                                labels[neighbor2] = 0

            # Marca como dominado todo vértice com label 0 que tenha vizinho com label 2
            for v in range(num_vertices):
                if labels[v] == 0 and not dominated[v]:
                    neighbors = np.where(adjacency[v] == 1)[0]
                    if np.any(labels[neighbors] == 2):
                        dominated[v] = True

            # Trata vértices restantes com label -1 ou 0 e não dominados
            for v in range(num_vertices):
                if (labels[v] == -1 or labels[v] == 0) and not dominated[v]:
                    neighbors = np.where(adjacency[v] == 1)[0]
                    if not np.any(labels[neighbors] == 2):
                        labels[v] = 1
                        dominated[v] = True

            solutions.append(labels.copy())

        return np.array(solutions)
    

    def degree_based_initialization(self):
        g = self.graph
        num_vertices = len(g.nodes)
        adjacency = g.adjacency_matrix
        degree = adjacency.sum(axis=1)

        # Inicializa labels e dominância
        labels = np.full(num_vertices, -1, dtype=int)
        dominated = np.zeros(num_vertices, dtype=bool)

        while np.any(labels == -1):
            unlabelled_idx = np.where(labels == -1)[0]
            if len(unlabelled_idx) == 0:
                break

            # Escolhe vértice de maior grau
            candidate_degrees = degree[unlabelled_idx]
            v = unlabelled_idx[np.argmax(candidate_degrees)]

            neighbors = np.where(adjacency[v] == 1)[0]

            # Trata vértice isolado
            if len(neighbors) == 0:
                labels[v] = 1
                dominated[v] = True
                continue

            # Verifica se pode ser 2
            neighbors_labels = labels[neighbors]
            neighbors_dominated = dominated[neighbors]

            can_be_2 = True
            # Se algum vizinho é 2, 1 ou é 0 já dominado, não pode ser 2
            if np.any(neighbors_labels == 2) or np.any(neighbors_labels == 1) or np.any((neighbors_labels == 0) & neighbors_dominated):
                can_be_2 = False

            if can_be_2 and not dominated[v]:
                labels[v] = 2
                dominated[v] = True
                # Vizinhos não rotulados viram 0 e são dominados
                unlabeled_neighbors = neighbors[labels[neighbors] == -1]
                labels[unlabeled_neighbors] = 0
                dominated[unlabeled_neighbors] = True
            elif not dominated[v]:
                labels[v] = 1
                dominated[v] = True
            else:
                if labels[v] == -1:
                    labels[v] = 0
        
        return labels
    

    def check_prd(self, solution):
        adjacency = self.graph.adjacency_matrix
        solution = np.asarray(solution)

        # Número de vizinhos 2 para cada vértice
        count_neighbors_2 = adjacency @ (solution == 2)

        # Para os vértices com label 0, exige count == 1
        zero_vertices = solution == 0
        if not np.all(count_neighbors_2[zero_vertices] == 1):
            return False

        return True
    

    def check_prd_v2(self, solution):
        adjacency = self.graph.adjacency_matrix
        solution = np.asarray(solution)

        count_neighbors_2 = adjacency @ (solution == 2)

        zero_vertices = solution == 0
        if not np.all(count_neighbors_2[zero_vertices] == 1):
            return False

        dominated = (solution == 1) | (solution == 2) | (count_neighbors_2 > 0)

        if not np.all(dominated):
            return False

        return True
    
    def check_prd_v3(self, solutions_2d):
        adjacency = self.graph.adjacency_matrix
        solutions_2d = np.asarray(solutions_2d)
        results = []

        for solution in solutions_2d:
            count_neighbors_2 = adjacency @ (solution == 2)
            zero_vertices = solution == 0
            if not np.all(count_neighbors_2[zero_vertices] == 1):
                results.append(False)
                continue

            dominated = (solution == 1) | (solution == 2) | (count_neighbors_2 > 0)
            if not np.all(dominated):
                results.append(False)
                continue

            results.append(True)
        return np.array(results)