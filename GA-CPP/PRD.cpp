#include "PRD.hpp"
#include "Solution.hpp"

PRD::PRD(Graph* g) {
    this->graph = g;
}

bool PRD::check_prd(Solution* sol) {
    PRD::restartGraph(); // Reinicia o grafo
    
    std::vector<int> s = sol->solution;
    int numNodes = sol->solution.size();

    // Rotula todo mundo
    for(int i = 0; i < numNodes; i++){
        this->graph->nodes[i]->label = s[i];
    }

    // Verificar se vertices com rotulo 0 tem 
    // exatemente um vizinho com rotulo 2
    for(int i = 0; i < numNodes; i++){
        if(this->graph->nodes[i]->label == 0){
            bool hasDominance = false;
            int howManyTwo = 0;
            for(Node* k : this->graph->nodes[i]->neighborhood){
                if(k->label == 2){
                    hasDominance = true;
                    howManyTwo++;
                    
                    if(howManyTwo > 1){
                        break;
                    }
                }
            }
            if(!hasDominance || howManyTwo != 1){
                return false;
            }
        }
    }
    return true;
}

Solution* PRD::greedy_initialization() {
    this->restartGraph(); // todos os vértices começam com label = -1 e não dominados

    // Comparador: nó com maior grau tem prioridade
    auto cmp = [](Node* a, Node* b) {
        return a->neighborhood.size() < b->neighborhood.size();
    };

    // Fila de prioridade (máximo no topo)
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

    // Inserir todos os nós na fila
    for (Node* node : this->graph->nodes){
        pq.push(node);
    }

    while (!pq.empty()) {
        Node* v = pq.top();
        pq.pop();

        // Se já foi rotulado, pula
        if (v->label != -1){
             continue;
        }
            

        // Trata vértice isolado
        if (v->neighborhood.empty()) {
            v->label = 1;
            v->isDominated = true;
            continue;
        }

        bool can_be_2 = true;
        std::vector<Node*> neighbors;

        // Analisa os vizinhos
        for (Node* neighbor : v->neighborhood) {
            neighbors.push_back(neighbor);

            if ((neighbor->label == 0 && neighbor->isDominated) ||
                neighbor->label == 1 || neighbor->label == 2){
                    can_be_2 = false;
                }
        }

        // Se puder ser 2, marca e domina os vizinhos
        if (can_be_2 && !v->isDominated) {
            v->label = 2;
            v->isDominated = true;

            for (Node* neighbor : neighbors) {
                if (neighbor->label == -1) {
                    neighbor->label = 0;
                    neighbor->isDominated = true;
                }
            }
        }
        // Caso contrário, rotula como 1
        else if (!v->isDominated) {
            v->label = 1;
            v->isDominated = true;
        }
        // Se já estava dominado, define como 0 se ainda não tiver rótulo
        else {
            if (v->label == -1) v->label = 0;
        }
    }

    // Monta a solução
    std::vector<int> labels;
    labels.reserve(this->graph->nodes.size());

    for (Node* node : this->graph->nodes) {
        labels.push_back(node->label);
    }

    return new Solution(labels, this);
}

std::vector<Solution*> PRD::randomized_initialization() {
    int num_vertices = this->graph->nodes.size();

    std::vector<int> indices(num_vertices);
    for (int i = 0; i < num_vertices; i++) indices[i] = i;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    std::vector<std::vector<int>> solutions;

    for (int i = 0; i < num_vertices / 2; ++i) { // Gera apenas metade

        this->restartGraph();

        Node* v = this->graph->nodes[indices[i]];

        if (v->label == -1 && !v->isDominated) {
            // Verifica se o vértice é isolado
            if (v->neighborhood.empty()) {
                v->label = 1;
                v->isDominated = true;
                continue;
            } else {
                v->label = 2;
                v->isDominated = true;

                for (Node* neighbor : v->neighborhood) {
                    if (neighbor->label == -1 && !neighbor->isDominated) {
                        neighbor->label = 0;
                        neighbor->isDominated = true;
                        for (Node* neighbor2 : neighbor->neighborhood) {
                            if (neighbor2->label == -1 && !neighbor2->isDominated) {
                                neighbor2->label = 0;
                            }
                        }
                    }
                }
            }
        }

        // Marca como dominado todo vértice com label 0 que tenha vizinho com label 2
        for (Node* node : this->graph->nodes) {
            if (node->label == 0 && !node->isDominated) {
                for (Node* neighbor : node->neighborhood) {
                    if (neighbor->label == 2) {
                        node->isDominated = true;
                        break;
                    }
                }
            }
        }

        // Trata vértices restantes com label -1 ou 0 e não dominados
        for (Node* node : this->graph->nodes) {
            if ((node->label == -1 || node->label == 0) && !node->isDominated) {
                bool has_label_2_neighbor = false;
                for (Node* neighbor : node->neighborhood) {
                    if (neighbor->label == 2) {
                        has_label_2_neighbor = true;
                        break;
                    }
                }
                if (!has_label_2_neighbor) {
                    node->label = 1;
                    node->isDominated = true;
                }
            }
        }

        // Salva a solução atual (cópia dos labels)
        std::vector<int> solution;
        for (Node* node : this->graph->nodes) {
            solution.push_back(node->label);
        }
        solutions.push_back(solution);
    }

    std::vector<Solution*> final;
    final.reserve(solutions.size());

    for (const std::vector<int>& sol : solutions) {
        Solution* aux = new Solution(sol, this);
        final.push_back(aux);
    }

    return final;
}

Solution* PRD::random_solution(){
    
    std::random_device rd;      // fonte de entropia
    std::mt19937 gen(rd());     // motor Mersenne Twister
    std::uniform_int_distribution<> dist(0, 2); // intervalo [0, 2]

    std::vector<int> sol;

    while(sol.size() < this->graph->numNodes){
        sol.push_back(dist(gen));
    }

    return new Solution(sol, this);
}

// Auxiliary functions
void PRD::restartGraph() {
    for(int i = 0; i < this->graph->numNodes; i++){
        this->graph->nodes[i]->label = -1;
        this->graph->nodes[i]->isDominated = false;
    }
}