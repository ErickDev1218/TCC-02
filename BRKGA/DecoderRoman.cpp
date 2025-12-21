
#include "DecoderRoman.h"
#include <numeric>
#include <algorithm>
#define DEBUG 1

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
 * realizando algumas trocas válidas de acordo com a dominação romana perfeita.
 */
void reduceWeight(const Graph& graph, std::vector< double >& chromosome, std::vector<int>& label, std::vector<int>& dominanceNumber) {
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
                change_key(chromosome, u, 1);

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
            change_key(chromosome, u, 0);
        }
    }
}

/**
 * @brief Esta função retorna o rótulo associado à chave aleatória passada como entrada
 * 
 * @param v chave aleatória
 * @return int rótulo
 */
int getLabel(double v){
    if(v >= 0 && v < 0.33){
        return 0;
    } else if(v >= 0.33 && v < 0.66){
        return 1;
    } else {
        return 2;
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
double DecoderRoman::decode(std::vector< double >& chromosome) const {
	const int n = g.getOrder();
    std::vector<int> f(n, 0); // pesos 0, 1, 2
    std::vector<int> dominanceNumber(n, 0); // contador de quantos dominadores o vertice tem, incluindo a si proprio
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
        // Se label == 2, ajustar a dominancia dos vizinhos
        if(label == 2){
            for(int u : g.getNeighbors(v)){
                dominated[u] = true;
                dominanceNumber[u]++;
            }
        }
    }

    // 2. Verificar se é uma solucao viavel e corrije quando nao for
    for(int v = 0; v < n; v++){
        // se for um vértice isolado, recebe rótulo 1
        if(g.getVertexDegree(v) == 0) {
            f[v] = 1;
            dominated[v] = true;
            dominanceNumber[v] = 1;
            change_key(chromosome, v, 1);
        }
        // Olhando apenas para os vertices com rotulo 0 sub ou sobredominados
        else if(f[v] == 0 && (dominanceNumber[v] == 0 || dominanceNumber[v] >= 2)){
            bool dominatedNeighbor = false;
            // Olhar a vizinhanca
            for(int u : g.getNeighbors(v)){
                if(dominated[u] == true){
                    dominatedNeighbor = true;
                }
            }
            // O vértice v poder ser sobredominado ou subdominado
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
            } 
            else {
                f[v] = 1;
                dominated[v] = true;
                dominanceNumber[v]++;
                change_key(chromosome, v, 1);
            }
        }
    }

    reduceWeight(g, chromosome, f, dominanceNumber);

    #if DEBUG
    checkPRD(g, f);
    #endif

    // 4. Calcula custo total
    double cost = 0.0;
    for(int v = 0; v < n; ++v) {
        cost += f[v];
        if(f[v] == 0) change_key(chromosome, v, 0);
    }
    return cost;
}
