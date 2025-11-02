#include "PRD.hpp"
#include "Solution.hpp"

PRD::PRD(Graph* g) {
    this->graph = g;
}

bool PRD::checkPRD(Solution* sol) {
    PRD::resetGraph(sol->solution); // Reset the graph
    // Check if every vertex with label 0 has
    // exacly one neighbor with label 2
    for(Node* u: this->graph->nodes){
        if((u->label == 0 && u->isDominated == false && u->dominatedFor == 0) || (u->label == 0 && u->isDominated == true && u->dominatedFor > 1)){
            return false;
        }
    }
    return true;
}

Solution* PRD::greedyInitialization() {
    this->restartGraph(); // Restart the graph

    // Vertex with bigger degree has priority
    auto cmp = [](Node* a, Node* b) {
        return a->neighborhood.size() < b->neighborhood.size();
    };

    // Priority queue (max on front)
    std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> pq(cmp);

    // Push every vertex on queue
    for (Node* node : this->graph->nodes){
        pq.push(node);
    }

    while (!pq.empty()) {
        Node* v = pq.top();
        pq.pop();

        if (v->label != -1){
             continue;
        }
        // Isoled vertex case
        if (v->neighborhood.empty()) {
            v->label = 1;
            v->isDominated = true;
            v->dominatedFor++;
            continue;
        }

        bool canBe2 = true;
        // Analysis the neighbors
        for (Node* neighbor : v->neighborhood) {
            if ((neighbor->label == 0 && neighbor->isDominated) ||
                neighbor->label == 1 || neighbor->label == 2){
                    canBe2 = false;
                }
        }
        // If can be 2, mark and domine the neighbors
        if (canBe2 && !v->isDominated) {
            v->label = 2;
            v->isDominated = true;
            v->dominatedFor++;
            // Mark his neighbors unlabeled as 0
            // and fix his attributes
            for (Node* neighbor : v->neighborhood) {
                if (neighbor->label == -1) {
                    neighbor->label = 0;
                    neighbor->isDominated = true;
                    neighbor->dominatedFor++;
                }
            }
        }
        // Else, mark as 1
        else if (!v->isDominated) {
            v->label = 1;
            v->isDominated = true;
            v->dominatedFor++;
        }
        // If already dominated, defines as 0 if don't have label yet
        else {
            if (v->label == -1) v->label = 0;
        }
    }
    // Create and return the new solution
    return new Solution(this->graph->getLabels(), this);
}

std::vector<Solution*> PRD::randomizedInitialization() {

    int numVertex = this->graph->numNodes;

    std::vector<int> indices(numVertex);
    for (int i = 0; i < numVertex; i++) indices[i] = i;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    std::vector<Solution*> final;
    final.reserve(numVertex / 2);

    // Create only the half
    for (int i = 0; i < numVertex / 2; ++i) {
        this->restartGraph();

        Node* v = this->graph->nodes[indices[i]];
        if (v->label == -1 && !v->isDominated) {
            // Check if that vertex its isolated
            if (v->neighborhood.empty()) {
                v->label = 1;
                v->isDominated = true;
                v->dominatedFor++;
                continue;
            } else {
                v->label = 2;
                v->isDominated = true;
                v->dominatedFor++;

                // Fix the neighborhood
                for (Node* neighbor : v->neighborhood) {
                    if (neighbor->label == -1 && !neighbor->isDominated) {
                        neighbor->label = 0;
                        neighbor->isDominated = true;
                        neighbor->dominatedFor++;
                        for (Node* neighbor2 : neighbor->neighborhood) {
                            if (neighbor2->label == -1 && !neighbor2->isDominated) {
                                neighbor2->label = 0;
                            }
                        }
                    }
                }
            }
        }

        // Mark as dominated every vertex with label = 0 that have a neighbor with label = 2
        for (Node* node : this->graph->nodes) {
            if (node->label == 0 && !node->isDominated) {
                for (Node* neighbor : node->neighborhood) {
                    if (neighbor->label == 2) {
                        node->isDominated = true;
                        node->dominatedFor++;
                        break;
                    }
                }
            }
        }

        // Fix vertex remaining with label = -1 or 0 and not dominated
        for (Node* node : this->graph->nodes) {
            if ((node->label == -1 || node->label == 0) && !node->isDominated) {
                node->label = 1;
                node->isDominated = true;
                node->dominatedFor++;
            }
        }

        final.push_back( new Solution(this->graph->getLabels(), this) );
    }

    return final;
}

Solution* PRD::randomSolution(){
    
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
        this->graph->nodes[i]->dominatedFor = 0;
    }
}

void PRD::resetGraph(std::vector<int> s){
    restartGraph(); // Set the graph as new
    for(int i = 0; i < this->graph->numNodes; i++){
        this->graph->nodes[i]->label = s[i]; // Label the vertex
        if(s[i] == 1){
            this->graph->nodes[i]->isDominated = true; // Dominates himself
            this->graph->nodes[i]->dominatedFor++; // Added +1 in dominance number
        }else if(s[i] == 2){
            this->graph->nodes[i]->isDominated = true; // Dominates himself
            this->graph->nodes[i]->dominatedFor++; // Added +1 in dominance number

            // For labels == 2, it's necessary to mark his neighbors as dominated and increase that dominance number
            for(Node* u : this->graph->nodes[i]->neighborhood){
                u->isDominated = true;
                u->dominatedFor++;
            }
        }
    }
}

Solution* PRD::fixSolution(Solution* sol){
    
    this->resetGraph(sol->solution); // Reset the graph
    Solution* s = this->reduceWeight(sol); // Call reduceWeight beforehand
    
    for(int i = 0; i < this->graph->nodes.size(); i++){
        Node* u = this->graph->nodes[i];
        if(u->label == 0) {
            // Check if have some vertex with label = 0 that
            // is dominated
            bool hasSomeDominated = false;
            for(Node* v: u->neighborhood){
                if(v->label == 0 && v->isDominated){
                    hasSomeDominated = true;
                }
            }
            // If u is dominated from unic vertex with label = 2
            // u can be keep having label = 0
            if(u->dominatedFor == 1){
                continue;
            } 
            // If u its not dominated and his neighbors are not dominated too
            else if(u->dominatedFor == 0 && !hasSomeDominated){

                u->label = 2;
                u->isDominated = true;
                u->dominatedFor++;

                s->solution[i] = 2;

                // Set his neighbors as dominated and increase his dominated number
                for(Node* v: u->neighborhood){
                    v->isDominated = true;
                    v->dominatedFor++;
                }
            }else{
                // Else, u needs to have label = 1 
                u->label = 1;
                u->dominatedFor++;
                u->isDominated = true;

                s->solution[i] = 1;
            }
        }
    }

    return this->reduceWeight(s);
}

Solution* PRD::reduceWeight(Solution* s){

    for(int i = 0; i < this->graph->numNodes; i++){
        Node* u = this->graph->nodes[i];

        if(u->label == 2){
            // Check if nodes with label = 2 
            // are dominating someone
            bool safe = true;
            for(Node* v: u->neighborhood){
                if(v->label == 0 && v->dominatedFor == 1){
                    safe = false;
                    break;
                }
            }
            // If is not dominating someone
            // he can ben = 1
            if(safe){
                u->label = 1;

                s->solution[i] = 1;

                // Fix his neighbors
                for(Node* w: u->neighborhood){
                    w->dominatedFor--;
                    if(w->label == 0 && w->dominatedFor ==0){
                        w->isDominated = false;
                    }
                }
            }
        }
    }

    // For every vertex with label = 1, 
    // if he its dominated for another vertex
    // with label = 2, so he can have label = 0
    for(int i = 0; i < this->graph->numNodes; i++){
        Node* v = this->graph->nodes[i];

        if(v->label == 1 && v->dominatedFor == 2){
            v->label = 0;
            v->dominatedFor--;

            s->solution[i] = 0;
        }
    }

    std::vector<int> aux = s->solution;
    delete s;
    return new Solution(aux, this);
}
