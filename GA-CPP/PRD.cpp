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
        this->graph->nodes[i]->dominanceNumber.clear();
    }
}

Solution* PRD::fixSolution(Solution* s){
    this->restartGraph(); // Coloca o grafo em estado inicial

    // Rotule o grafo
    for(int i = 0; i < this->graph->numNodes; i++){
        this->graph->nodes[i]->label = s->solution[i];
        if(s->solution[i] == 1){
            this->graph->nodes[i]->isDominated = true; // Ele se domina
            // Nao vou dar um push_back no dominanceNumber pois esse campo é usado para f(v) == 0
        }else if(s->solution[i] == 2){
            this->graph->nodes[i]->isDominated = true;
            // Para f(v) == 2 eh necessario marcar os vizinhos 0 como dominados e dar um push_back
            // no dominanceNumber
            for(Node* u : this->graph->nodes[i]->neighborhood){
                u->isDominated = true;
                u->dominanceNumber.push_back(true);
            }
        }
    }
    // Acredito que nesse ponto o grafo estara rotulado e 
    // os vertices 0 que tem mais de um vizinho com rotulo 2
    // tem dominanceNumber.size() >= 2 -> trocar dominanceNumber por um int depois
    std::vector<Node*> zeroNodesWithProblems;
    
    // Olhe todos os vértices com f(v) == 0 e com dominanceNumber >= 2 ou nao dominados
    for(Node* u : this->graph->nodes){
        if(u->label == 0 && (u->dominanceNumber.size() >= 2 || !u->isDominated)){
            zeroNodesWithProblems.push_back(u);
        }
    }
    
    // Aqui temos duas situacoes para nao ser um PRD
    for(Node* u : zeroNodesWithProblems){
        // Caso 1: nao ha vizinho com rotulo 2 e u está desprotegido.
        if(u->isDominated == false){
            // Olhar a vizinhança completa de u
            bool canBeTwo = true;
            std::vector<Node*> aux;
            for(Node* v : u->neighborhood){
                aux.push_back(v);
                // Se houver um vizinho 0 sem estar protegido, talvez u precise ser 2
                if(v->isDominated == true){
                    canBeTwo = false;
                }
            }
            if(canBeTwo){
                u->label = 2;
                u->isDominated = true;
                u->dominanceNumber.push_back(true);
                
                for(Node * x : aux){
                    x->isDominated = true;
                    x->dominanceNumber.push_back(true); // !!! Alterar depois !!!
                }
                
            } else {
                u->label = 1;
                u->isDominated = true;
                u->dominanceNumber.push_back(true); 
            }
        }
        // Caso 2: u esta dominado por mais de um vizinho com rotulo 2
        else if(u->dominanceNumber.size() >= 2){
            // Descubra quem sao os vizinhos de u com rotulo 2
            std::vector<Node*> labelEqualsTwo;
            for(Node* v : u->neighborhood){
                if(v->label == 2){
                    labelEqualsTwo.push_back(v);
                }
            }
            // Para os vertices com rotulos 2, descubra quem tem menor grau
            Node* minDegree = labelEqualsTwo[0];
            for(int i = 1; i < labelEqualsTwo.size(); i++){
                if(minDegree->neighborhood.size() > labelEqualsTwo[i]->neighborhood.size()){
                    minDegree = labelEqualsTwo[i];
                }
            }

            // Descobrir quem eh o vertice de menor grau eh importante
            // pois provavelmente re-rotular ele seja mais simples

            // Casos que prevejo:
            
            // Caso 1: todos os seus vizinhos ja estao dominados por outro vertice
            // entao esse vertice precisa ser 1 ou 0 (ele pode ter um vizinho 2, dado
            // que ter 2----2 nao eh problema aqui)

            // Caso 2: acho que o caso acima eh suficiente

            // Primeiro passo: conhecer a vizinha desse vertice ja com algumas inferencias
            bool canBeZero = true;
            bool needToBeOne = false;
            int howManyTwo = 0;
            int howManyDefenseless = 0;
            
            for(Node* v : minDegree->neighborhood){
                if(v->label == 2){
                    howManyTwo++;
                }
                if(v->isDominated == false){
                   howManyDefenseless++;
                }
            }

            if(howManyTwo == 1 && howManyDefenseless == 0){
                minDegree->label = 0;
                minDegree->dominanceNumber.pop_back(); // Remover 1 da contagem
                minDegree->isDominated = true; // Talvez ja estivesse com true, mas garanto aqui
                
            }else if(howManyTwo > 1 || howManyTwo < 1){
                minDegree->label = 1;
                minDegree->isDominated = true;
            
            }
        }
    }

    // Agora com a solucao corrigida, deleta a passada e retorna uma nova
    delete s;
    std::vector<int> n;
    n.reserve(this->graph->numNodes);
    for(Node* u : this->graph->nodes){
        n.push_back(u->label);
    }
    return new Solution(n, this);
}

