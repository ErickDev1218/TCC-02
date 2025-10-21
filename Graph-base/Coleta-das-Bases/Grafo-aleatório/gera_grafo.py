import networkx as nx
import matplotlib.pyplot as plt

# G(n, p): grafo aleatório de Erdős–Rényi

n = 10   # número de vértices
p = 0.3  # probabilidade de haver uma aresta entre dois vértices

G = nx.erdos_renyi_graph(n, p)

# Exibir grafo
nx.draw(G, with_labels=True, node_color='lightblue')
plt.show()
