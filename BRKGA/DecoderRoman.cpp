
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
    if(new_label == 0){
        chromosome[v] = 0.16;
    } else if(new_label == 1){
        chromosome[v] = 0.49;
    }else{
        chromosome[v] = 0.82;
    }
}

/**
 * @brief Esta função recebe como entrada um cromossomo e tenta reduzir o peso da solução 
 * realizando algumas trocas válidas:
 * - se uv in E(G) e f(u)=2 e f(v)=1, então redefinimos o peso de v como f(v)=0.
 * - se u tem degree(u)>=2 e f(u)=1 e u tem pelo menos 2 vizinhos w com f(w)=1, então redefinimos 
 *   o peso de u como f(u)=2 e os seus vizinhos com rótulo 1 recebem rótulo 0.
 */
void reduceWeight(const Graph& graph, std::vector< double >& chromosome, std::vector<int>& label, std::vector<int>& dominanceNumber) {
    const int n = graph.getOrder();

    for(int u = 0; u < n; u++){
        if(label[u] == 2){
            // Verificar se v esta dominando alguem
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
                change_key(chromosome, u, 1);

                //Corrije seus vizinhos
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
            change_key(chromosome, u, 0);
        }
    }
}

int getLabel(double v){
    if(v >= 0 && v < 0.33){
        return 0;
    }else if (v >= 0.33 && v < 0.66){
        return 1;
    }else {
        return 2;
    }
}

/**
 * @brief decoder
 */
double DecoderRoman::decode(std::vector< double >& chromosome) const {
	const int n = g.getOrder();
    std::vector<int> f(n, 0); // pesos 0, 1, 2
    std::vector<int> dominanceNumber(n, 0); // contador de quantos dominadores o vertice tem incluindo a si proprio
    std::vector<bool> dominated(n, false);
    // 1. Rotular o grafo
    for(int v = 0; v < n; v++){
        int label = getLabel(chromosome[v]); // Transforma um valor continuo em um discreto
        f[v] = label; // Rotula o vertice

        // Domina a si mesmo
        if(label == 1 || label == 2){
            dominated[v] = true;
            dominanceNumber[v]++;
        }
        // Se label == 2, ajustar a dominancia dos vizinho
        if(label == 2){
            for(int u : g.getNeighbors(v)){
                dominated[u] = true;
                dominanceNumber[u]++;
            }
        }
    }
    // 2. Verificar se é uma solucao viavel e corrije quando nao for
    for(int v = 0; v < n; v++){
        // Olhando apenas para os vertices com rotulo 0 sub ou sobredominados
        if(f[v] == 0 && (dominanceNumber[v] == 0 || dominanceNumber[v] >= 2)){
            bool dominatedNeighbor = false;
            // Olhar a vizinhanca
            for(int u : g.getNeighbors(v)){
                if(dominated[u] == true){
                    dominatedNeighbor = true;
                }
            }
            // 0 poder ser sobredominado ou subdominado
            // Caso seja subdominado e seus vizinhos também não forem dominados
            if(dominanceNumber[v] == 0 && !dominatedNeighbor){
                f[v] = 2;
                dominated[v] = true;
                dominanceNumber[v]++;
                change_key(chromosome, v, 2);

                // Atualizar a vizinhaca de v
                for(int u : g.getNeighbors(v)){
                    dominated[u] = true;
                    dominanceNumber[u]++;
                }
            }else {
                f[v] = 1;
                dominated[v] = true;
                dominanceNumber[v]++;
                change_key(chromosome, v, 1);
            }
        }
    }
    reduceWeight(g, chromosome, f, dominanceNumber);
    // 4. Calcula custo total
    double cost = 0.0;
    for(int v = 0; v < n; ++v) {
        cost += f[v];
        if(f[v] == 0) change_key(chromosome, v, 0);
    }
    return cost;
}
