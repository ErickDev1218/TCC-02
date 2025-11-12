#include <iostream>
#include <vector>
#include <map>
#include <numeric>
#include <fstream>
#include <filesystem>
#include "gurobi_c++.h"

namespace fs = std::filesystem;

// Definição de tipos para facilitar a leitura
using Vertex = int;
using Graph = std::map<Vertex, std::vector<Vertex>>; // Adjacency list: Vertex -> list of neighbors

// Função para construir o modelo
void solve_optimization_problem(const Graph& G, const std::string& graphName) {
    std::cout << graphName << "\n";
    try {
        // 1. Configurar o Ambiente e o Modelo
        GRBEnv env = GRBEnv(true);
        env.set("LogFile", "optimization_log.log");
        env.start();

        GRBModel model = GRBModel(env);
        model.getEnv().set(GRB_IntParam_OutputFlag, 0);
        model.set(GRB_StringAttr_ModelName, "PLI_Sem_Linearizacao");

        // Obter o conjunto de vértices
        std::vector<Vertex> V;
        for (const auto& pair : G) {
            V.push_back(pair.first);
        }

        // 2. Criar as Variáveis de Decisão
        std::map<Vertex, GRBVar> x_vars;
        std::map<Vertex, GRBVar> y_vars;

        for (Vertex v : V) {
            // x_v (5.2d): Variável binária
            x_vars[v] = model.addVar(0, 1, 0, GRB_BINARY, "x_" + std::to_string(v));
            // y_v (5.2d): Variável binária
            y_vars[v] = model.addVar(0, 1, 0, GRB_BINARY, "y_" + std::to_string(v));
        }

        // 3. Definir a Função Objetivo (5.2a): minimize sum(x_v + 2*y_v)
        GRBQuadExpr obj = 0;
        for (Vertex v : V) {
            obj += x_vars[v];
            obj += 2 * y_vars[v];
        }
        model.setObjective(obj, GRB_MINIMIZE);

        // 4. Adicionar as Restrições

        // Restrição (5.2c): x_v + y_v <= 1, para todo v in V(G)
        for (Vertex v : V) {
            GRBLinExpr lhs = x_vars[v] + y_vars[v];
            model.addConstr(lhs <= 1, "R52c_" + std::to_string(v));
        }

        // Restrição (5.2b): Restrição Não-Linear
        // 2*x_v + 2*y_v + (1-x_v)*(1-y_v) * sum_{u in N(v)} 2*y_u = 2, para todo v in V(G)
        for (Vertex v : V) {
            // Expressão Linear de 2*x_v + 2*y_v
            GRBLinExpr linear_part = 2 * x_vars[v] + 2 * y_vars[v];

            // Termo Não-Linear: (1 - x_v)*(1 - y_v) * sum_{u in N(v)} 2*y_u
            
            // Passo A: Calcular o termo sum_{u in N(v)} 2*y_u (Linear)
            GRBLinExpr sum_y_u = 0;
            if (G.count(v)) { // Verificar se o vértice existe no mapa
                for (Vertex u : G.at(v)) {
                    // Garantir que u é um vértice válido
                   if (y_vars.count(u)) { 
                        sum_y_u += 2 * y_vars.at(u);
                   }
                }
            }

            // Passo B: Calcular o produto das duas variáveis binárias: (1 - x_v)*(1 - y_v)
            // O Gurobi aceita o produto de variáveis GRBVar * GRBVar, resultando em GRBQuadExpr
            GRBQuadExpr product_xy = (1 - x_vars[v]) * (1 - y_vars[v]);

            // Passo C: Calcular o produto do termo quadrático pelo termo linear
            // OBS: O Gurobi C++ não permite a multiplicação direta de GRBQuadExpr * GRBLinExpr
            // para um MIQP simples, pois resultaria em um termo cúbico (MINLP).
            
            // Para manter o MIQP/MINLP resolvível pelo Gurobi, *devemos* usar a função 
            // GRBModel::addGenConstrIndicator() ou GRBModel::addGenConstrMultiVar().
            // Alternativamente, se (1 - x_v)*(1 - y_v) é 0 ou 1, e o Gurobi aceita a 
            // multiplicação por um termo linear (que é o caso no Gurobi moderno, usando 
            // o recurso General Constraint para termos MINLP), a sintaxe abaixo funciona.
            // Para MIQP (restrição quadrática): O Gurobi converterá internamente a restrição
            // não-linear em termos que ele consiga processar, seja via MIQP ou MINLP (se ativo).
            
            // Para forçar o Gurobi a aceitar esta construção como MINLP:
            // O Gurobi consegue lidar com termos cúbicos ou de grau superior quando 
            // *apenas variáveis binárias* estão sendo multiplicadas, ou quando a 
            // opção NonConvex está definida.
            
            // Vamos usar a sintaxe direta e confiar na capacidade de MINLP do Gurobi:
            // É importante setar o parâmetro NonConvex para 2, para que o Gurobi aceite 
            // a multiplicação de um termo quadrático por um linear (que gera um termo cúbico).
            model.set(GRB_IntParam_NonConvex, 2);
            
            // Termo Completo (Cúbico): (1 - x_v)*(1 - y_v) * sum_{u in N(v)} 2*y_u
            GRBQuadExpr nonlinear_term = (1 - x_vars[v]) * (1 - y_vars[v]);
            GRBLinExpr rhs_expr = sum_y_u;

            // O Gurobi não aceita GRBQuadExpr * GRBLinExpr diretamente.
            // Precisamos introduzir uma variável auxiliar Z_v para o produto (1-x_v)(1-y_v) * sum.
            
            // Simplificando o Termo
            // Z_v = (1 - x_v)(1 - y_v) * sum_{u in N(v)} 2*y_u
            
            // Como (1-x_v) e (1-y_v) são binárias, (1-x_v)(1-y_v) também é binário.
            // Seja w_v = (1-x_v)(1-y_v).
            // W_v = 1 SOMENTE SE x_v=0 E y_v=0.
            
            // Adicionar uma variável binária auxiliar w_v
            GRBVar w_v = model.addVar(0, 1, 0, GRB_BINARY, "w_" + std::to_string(v));
            
            // Restrições de Equivalência para w_v = (1 - x_v)(1 - y_v)
            // (Esta é a linearização *padrão* de um produto de 2 binárias, mas para o termo completo
            // a restrição 5.2b *não* se torna linear, então o princípio do usuário é mantido.)
            // Gurobi pode usar General Constraints aqui, o que é mais limpo:
            // model.addGenConstrAnd(w_v, std::vector<GRBVar>{1-x_vars[v], 1-y_vars[v]}, "And_wv_" + std::to_string(v));
            // *O Gurobi C++ não suporta a negação (1-x_v) diretamente em GenConstrAnd. Usaremos a forma tradicional.*
            
            // w_v <= 1 - x_v  => w_v + x_v <= 1
            model.addConstr(w_v + x_vars[v] <= 1, "W_R1_" + std::to_string(v));
            // w_v <= 1 - y_v  => w_v + y_v <= 1
            model.addConstr(w_v + y_vars[v] <= 1, "W_R2_" + std::to_string(v));
            // w_v >= (1 - x_v) + (1 - y_v) - 1 => w_v + x_v + y_v >= 1
            model.addConstr(w_v + x_vars[v] + y_vars[v] >= 1, "W_R3_" + std::to_string(v));


            // A restrição 5.2b se torna: 2*x_v + 2*y_v + w_v * sum_{u in N(v)} 2*y_u = 2
            // Que ainda é um **MINLP** por causa do produto da binária w_v pela expressão linear sum_y_u.
            
            // Restrição (5.2b) Final: 2*x_v + 2*y_v + w_v * sum_{u in N(v)} 2*y_u = 2
            // Usaremos a General Constraint do Gurobi (GRB_GENCONSTR_MULTICOND) para o produto:
            // Prod_v = w_v * sum_{u in N(v)} 2*y_u
            
            // Z_v (variável auxiliar contínua para o produto)
            GRBVar Z_v = model.addVar(-GRB_INFINITY, GRB_INFINITY, 0, GRB_CONTINUOUS, "Z_" + std::to_string(v));
            
            // Adicionar Restrição de Produto: Z_v = w_v * sum_y_u
            // O Gurobi suporta o produto de uma variável binária (w_v) por uma expressão linear (sum_y_u) 
            // através de **Restrições Indicadoras (General Constraints)**.
            // Se w_v = 1, então Z_v = sum_y_u
            model.addGenConstrIndicator(w_v, 1, Z_v == sum_y_u, "Indicator_1_" + std::to_string(v));
            // Se w_v = 0, então Z_v = 0
            model.addGenConstrIndicator(w_v, 0, Z_v == 0, "Indicator_0_" + std::to_string(v));


            // Restrição 5.2b (Agora Linear)
            // 2*x_v + 2*y_v + Z_v = 2
            GRBLinExpr R52b_lhs = linear_part + Z_v;
            model.addConstr(R52b_lhs == 2, "R52b_" + std::to_string(v));
        }

        // 5. Otimizar o Modelo
        model.optimize();

        // 6. Exibir os Resultados
        if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {

            // std::cout << "\n*** Solução Ótima Encontrada para o grafo " << graphName << " ***" << std::endl;
            // std::cout << "Valor Objetivo: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;
            
            // std::cout << "Variáveis:" << std::endl;
            // for (Vertex v : V) {
            //     std::cout << "x_" << v << " = " << x_vars[v].get(GRB_DoubleAttr_X) 
            //               << ", y_" << v << " = " << y_vars[v].get(GRB_DoubleAttr_X) 
            //               << std::endl;
            // }
            
            std::ofstream logFile("Gurobi_PRD_solutions.log", std::ios::app); // abre em modo "append"
            if (logFile.is_open()) {
                logFile << graphName<< ";" << model.get(GRB_DoubleAttr_ObjVal) << ";" << model.get(GRB_DoubleAttr_Runtime) << "\n";
                logFile.close();
            }
        } else {
            std::cout << "\n*** Solução Não Ótima Encontrada (Status: " 
                      << model.get(GRB_IntAttr_Status) << ") ***" << std::endl;
        }

    } catch (GRBException e) {
        std::cout << "Erro Gurobi: " << e.getErrorCode() << " - " << e.getMessage() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Erro padrão: " << e.what() << std::endl;
    }
}

void readGraphFromFile(const std::string& path) {
    std::string graphName = fs::path (path).stem().string();

    // if(graphName != "1138_bus") return;

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + path);
    }

    int num_vertex, num_edges;

    file >> num_vertex >> num_edges;

    Graph g;

    for(int v = 0; v < num_vertex; v++){
        g[v] = {};
    }
    
    // vector<pair<int, int>> edges;
    // edges.reserve(num_edges);

    int u, v;
    while (file >> u >> v) {
        // edges.push_back(make_pair(u,v));
        g[u].push_back(v);
        g[v].push_back(u);
    }

    // Graph* g = new Graph(num_vertex, edges, fs::path (path).stem().string());

    file.close();

    // GeneticAlgorithm* GA = new GeneticAlgorithm(g, params.populationFactor, params.tournamentSize, params.maxStagnant, params.mutationRate, params.elitismRate, params.crossoverRate, params.generations);
    
    // GA->gaFlow();

    // delete GA;
    
    // Mostra o grafo
    // for (const auto& [v, adj] : g) {
    //     std::cout << v << ": ";
    //     for (int neighbor : adj) {
    //         std::cout << neighbor << " ";
    //     }
    //     std::cout << "\n";
    
    // }
    solve_optimization_problem(g, graphName);
}

void readAllGraphsFromDir(const std::string& dirPath) {
    // const std::string& dirPath = params.file_path;
    
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".txt") {
            // std::cout << "📄 Reading: " << entry.path().filename() << std::endl;
            try {
                readGraphFromFile(entry.path().string());
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
}

std::string parse_args(int argc, char* argv[]){
    if(argc < 1){
        std::cerr << "The file path its required. \n";
        exit(1);
    }

    return argv[1];
}
// Exemplo de Uso (Main)
int main(int argc, char *argv[]) {
    // Exemplo de Grafo Simples: V={1, 2, 3}, E={(1,2), (2,3)}
    // Graph G_example = {
    //     {0, {1,6,9,5}},
    //     {1, {0, 2}},
    //     {2, {1, 3}},
    //     {3, {2, 4, 8}},
    //     {4, {3, 5, 7}},
    //     {5, {4, 0, 7}},
    //     {6, {0, 8, 7}},
    //     {7, {4, 5, 6, 8}},
    //     {8, {3, 6, 7}},
    //     {9, {0}},
    // };
    //a 0
    //b 1
    //c 2
    //d 3
    //e 4
    //f 5
    //g 6
    //h 7 
    //i 8
    //j 9

    std::string filepath = parse_args(argc, argv);
    // std::cout << filepath << std::endl;
    readAllGraphsFromDir(filepath);
    // solve_optimization_problem(G_example);

    return 0;
}