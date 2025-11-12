
#include "DecoderRoman.h"
#include <numeric>

/**
 * @brief Esta função recebe como entrada:
 * 1. um cromossomo de chaves aleatórias no intervalo [0,1);
 * 2. um vértice v, com v = 0,1,...,n-1;
 * 3. o rótulo new_label do vértice v, tal que new_label = 0,1,2.
 * 
 * Ela muda a chave aleatória contida em chromosome[v] de acordo com as seguintes regras:
 * 
 * se new_label = 0, então 0 <= chromosome[v] < 0.33;
 * se new_label = 1, então 0.33 <= chromosome[v] < 0.66;
 * se new_label = 2, então 0.66 <= chromosome[v];
 */
void change_key(std::vector< double >& chromosome, int v, int new_label) {
    if(new_label == 0) {
        if(chromosome[v] >= 0.66) {
            chromosome[v] = chromosome[v] - 0.66;
        } else if(chromosome[v] >= 0.33) {
            chromosome[v] = chromosome[v] - 0.33;
        }
    } else if(new_label == 1) {
        if(chromosome[v] >= 0.66) {
            chromosome[v] = chromosome[v] - 0.33;
        } else if(chromosome[v] < 0.33) {
            chromosome[v] = chromosome[v] + 0.33;
        }
    } else if(new_label == 2) {
        if(chromosome[v] < 0.33) {
            chromosome[v] = chromosome[v] + 0.66;
        } else if(chromosome[v] < 0.66) {
            chromosome[v] = chromosome[v] + 0.33;
        }
    }
}

/**
 * @brief Esta função recebe como entrada um cromossomo e tenta reduzir o peso da solução 
 * realizando algumas trocas válidas:
 * - se uv in E(G) e f(u)=2 e f(v)=1, então redefinimos o peso de v como f(v)=0.
 * - se u tem degree(u)>=2 e f(u)=1 e u tem pelo menos 2 vizinhos w com f(w)=1, então redefinimos 
 *   o peso de u como f(u)=2 e os seus vizinhos com rótulo 1 recebem rótulo 0.
 */
void reduce_weight_heuristic(const Graph& graph, std::vector< double >& chromosome, std::vector<int>& color) {
	for(size_t vertex : graph.getVertices()) {
		if(color[vertex] == 2) {
			for(size_t neighbor : graph.getNeighbors(vertex)) {
				if(color[neighbor] == 1) {
					color[neighbor] = 0;
                    change_key(chromosome, neighbor, 0);
				}
			}
		}
		else if(color[vertex] == 1 && graph.getVertexDegree(vertex) >= 2) {
			size_t total_with_label_one = 0;
			std::unordered_set<size_t> chosen_neighbors;
			for(size_t neighbor : graph.getNeighbors(vertex)) { 
				if(color[neighbor] == 1) {
					total_with_label_one++;
					chosen_neighbors.insert(neighbor);
				}
			}
			if(total_with_label_one >= 2) {
				color[vertex] = 2;
                change_key(chromosome, vertex, 2);
				for(size_t w : chosen_neighbors) {
					color[w] = 0;
                    change_key(chromosome, w, 0);
				}
			}
		} 
	}
}


/**
 * @brief decoder
 */
double DecoderRoman::decode(std::vector< double >& chromosome) const {
	const int n = g.getOrder();
    std::vector<int> f(n, 0); // pesos 0, 1, 2
    std::vector<bool> dominated(n, false);

    // 1. ordena os vértices pela chave, menor primeiro
    std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(),
        [&](int a, int b){ return chromosome[a] < chromosome[b]; });

    // 2. Atribui valores com base na dominância local
    for(int v : order) {
        bool isDominated = dominated[v];
        bool hasUndominatedNeighbor = false;

        for(int u : g.getNeighbors(v)) {
            if(!dominated[u]) {
                hasUndominatedNeighbor = true;
                break;
            }
        }

        if(g.getVertexDegree(v) == 0) {
            f[v] = 1;
            dominated[v] = true;
            change_key(chromosome, v, 1);
        } else if(!isDominated && hasUndominatedNeighbor) {
            // Atribui peso 2: protege a si mesmo e aos vizinhos
            f[v] = 2;
            dominated[v] = true;
            for(int u : g.getNeighbors(v)) { 
                dominated[u] = true; 
            }
            change_key(chromosome, v, 2);
        }
        else if(!isDominated && !hasUndominatedNeighbor) {
            // Coloca 1: protege apenas a si
            f[v] = 1;
            dominated[v] = true;
            change_key(chromosome, v, 1);
        }
        else if(isDominated && hasUndominatedNeighbor) {
            // Vizinhos ainda não dominados --- ele pode ajudar
            // mas com custo menor: escolha 1 com probabilidade leve
            if(chromosome[v] < 0.3) {
                f[v] = 1;
                change_key(chromosome, v, 1);
            }
        }
    }

    // 3. Reparo: garantir que todo 0 tenha vizinho 2
    for(int v = 0; v < n; ++v) {
        if(f[v] == 0) {
            bool ok = false;
            for(int u : g.getNeighbors(v)) {
                if(f[u] == 2) { 
                    ok = true; 
                    break; 
                }
            }
            if(!ok) {
                // escolhe o vizinho com menor chave para virar 2
                int best = -1;
                double bestKey = 2.0;
                for(int u : g.getNeighbors(v)) {
                    if(chromosome[u] < bestKey) {
                        best = u;
                        bestKey = chromosome[u];
                    }
                }
                if(best != -1) {
                    f[best] = 2;
                    dominated[best] = true;
                    for(int w : g.getNeighbors(best)) dominated[w] = true;
                    change_key(chromosome, best, 2);
                } else {
                    // vértice isolado: precisa de proteção própria
                    f[v] = 1;
                    dominated[v] = true;
                    change_key(chromosome, v, 1);
                }
            }
        }
    }

    reduce_weight_heuristic(g, chromosome, f);

    // 4. Calcula custo total
    double cost = 0.0;
    for(int v = 0; v < n; ++v) {
        cost += f[v];
        if(f[v] == 0) change_key(chromosome, v, 0);
    }

    return cost;
}
