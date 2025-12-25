
#include "DecoderRoman.h"

/**
 * @brief Esta função recebe como entrada um cromossomo e tenta reduzir o peso da solução 
 * realizando algumas trocas válidas:
 * - se uv in E(G) e f(u)=2 e f(v)=1, então redefinimos o peso de v como f(v)=0.
 * - se u tem degree(u)>=2 e f(u)=1 e u tem pelo menos 2 vizinhos w com f(w)=1, então redefinimos 
 *   o peso de u como f(u)=2 e os seus vizinhos com rótulo 1 recebem rótulo 0.
 */
void reduce_weight_heuristic(const Graph& graph, std::vector<int>& label, std::vector<int>& dominanceNumber) {
    const int n = graph.getOrder();
    for(int u = 0; u < n; u++){
        if(label[u] == 2){
            // Verificar se u é um dominador privado de algum de seus vizinhos
            bool safe = true;
            for(int v : graph.getNeighbors(u)){
                if(label[v] == 0 && dominanceNumber[v] == 1){
                    safe = false;
                    break;
                }
            }

            if(safe){
                // Trocar o rotulo de 2 para 1
                label[u] = 1;
                // Corrije o dominanceNumber dos vizinhos de u
                for(int v : graph.getNeighbors(u)){
                    dominanceNumber[v]--;
                }
            }
        }
    }

    // Para todo vertice com rotulo 1
    // se ele for dominado por si e outro vertice com rotulo 2
    // entao ele pode ter rotulo 0
    for(int u = 0; u < n; u++){
        if(label[u] == 1 && dominanceNumber[u] == 2){
            label[u] = 0;
            dominanceNumber[u]--;
        }
    }
}

void checkPRD(const Graph& g, const std::vector<int>& f){
    int n = g.getOrder();
    for(int u = 0; u < n; u++){
        if(f[u] == 0){
            int count = 0;
            for(int v : g.getNeighbors(u)){
                if(f[v] == 2){
                    count++;
                }
                if(count >= 2){
                    std::cout << "======>>> Not feasible <<<=========" << std::endl;
                    return;
                }
            }
            if(count == 0){
                std::cout << "======>>> Not feasible <<<=========" << std::endl;
                return;
            }
        }
    }
}

/**
 * @brief decoder
 */
double DecoderRoman::decode(const std::vector< double >& chromosome) const {

    const int n = g.getOrder();
	std::vector<int> order(n);
    std::iota(order.begin(), order.end(), 0);
    
    std::sort(order.begin(), order.end(),
    [&](int a, int b){
        return chromosome[a] > chromosome[b];
    });
    
    std::vector<int> f(n, 0);
    std::vector<int> dominanceNumber(n, 0);
    std::vector<bool> dominated(n, false);
    
    
    for(int idx = 0; idx < n; idx++){
        int u = order[idx];

        if(f[u] != 0) continue;

        if(dominanceNumber[u] == 1){
            continue;
        }

        if(dominanceNumber[u] >= 2){
            f[u] = 1;
            dominated[u] = true;
            dominanceNumber[u]++;
            continue;
        }

        bool hasDominatedNeighbor = false;
        for(int v : g.getNeighbors(u)){
            if(f[v] == 0 && dominated[v]){
                hasDominatedNeighbor = true;
                break;
            }
        }
        if(!hasDominatedNeighbor){
            f[u] = 2;
            dominated[u] = true;
            dominanceNumber[u]++;

            for(int v : g.getNeighbors(u)){
                dominated[v] = true;
                dominanceNumber[v]++;
            }
        } else {
            f[u] = 1;
            dominated[u] = true;
            dominanceNumber[u]++;
        }
    }

    reduce_weight_heuristic(g, f, dominanceNumber);

    //  checkPRD(g, f);

    double cost = 0.0;
    for(int v = 0; v < n; v++){
        cost += f[v];
    }

    return cost;

}
