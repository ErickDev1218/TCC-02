import os
import networkx as nx
import random

# Configuracoes
n = [500, 1000, 1500, 2000, 3000]
densities = [0.2, 0.4, 0.6, 0.8]
graph_per_combination = 5

# Diretorio de saida
OUTPUT_DIR = "base_final"

def ensure_dir(path):
    """Garante que o diretório de saída exista."""
    if not os.path.exists(path):
        os.makedirs(path)

def generator(path):
    ensure_dir(path)

    for num_vertex in n:
        for d in densities:
            add = 0.02
            for i in range(graph_per_combination):
                value = round(d + add, 2)
                max_edges = num_vertex * (num_vertex - 1) // 2
                num_edges = int(value * max_edges)
                G = nx.gnm_random_graph(num_vertex, num_edges)

                # Garante que é um grafo sem arestas múltiplas
                if isinstance(G, nx.MultiGraph):
                    G = nx.Graph(G)

                # Remove laços
                G.remove_edges_from(nx.selfloop_edges(G))

                # Relabel para 0-based consecutivo
                mapping = {old_label: new_label for new_label, old_label in enumerate(sorted(G.nodes()))}
                G = nx.relabel_nodes(G, mapping)

                filename = f"random_graph_{num_vertex}_{value}_{i+1}.txt"
                filepath = os.path.join(path, filename)

                with open(filepath, "w") as f_out:

                    qtd_vertices = G.number_of_nodes()
                    qtd_arestas = G.number_of_edges()

                    # Primeira linha: número de vértices e arestas
                    f_out.write(f"{qtd_vertices} {qtd_arestas}\n")

                    for u, v in G.edges():
                        f_out.write(f"{u} {v}\n")
                add += 0.02


if __name__ == "__main__":
    generator(OUTPUT_DIR)
