import numpy as np

class Graph:
    def __init__(self, nodes, edges):
        self.nodes = np.array(nodes)
        self.edges = np.array(edges)
        self.adjacency_matrix = self.create_adjacency_matrix()

    def create_adjacency_matrix(self):
        size = len(self.nodes)
        matrix = np.zeros((size, size), dtype=int)
        for edge in self.edges:
            matrix[edge[0], edge[1]] = 1
            matrix[edge[1], edge[0]] = 1
        return matrix

    def add_node(self, node):
        self.nodes = np.append(self.nodes, node)
        new_size = len(self.nodes)
        new_matrix = np.zeros((new_size, new_size), dtype=int)
        new_matrix[:np.size, :np.size] = self.adjacency_matrix
        self.adjacency_matrix = new_matrix

    def add_edge(self, edge):
        self.edges = np.append(self.edges, [edge], axis=0)
        self.adjacency_matrix[edge[0], edge[1]] = 1
        self.adjacency_matrix[edge[1], edge[0]] = 1  # Assuming undirected graph

    def get_neighbors(self, node):
        return np.where(self.adjacency_matrix[node] == 1)[0]