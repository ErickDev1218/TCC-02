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

struct Result {
    std::string graph_name = "";
    int node_count = -1;
    int edge_count = -1;
    float graph_density = -1;
    int objValue = -1;
    double elapsed_time = -1;
    bool isOptimal = false;
};

struct Parameters {
    std::string file_path;
    std::string output_file = "results.csv";
};

Parameters parse_args(int argc, char* argv[]);
void ensure_csv_header(const std::string &filename);
void write_result_to_csv(const std::string &filename, const Result &result);
void readGraphAndSolve(const std::string& path, const std::string& output);
void solvePRD(const Graph& G, Result& res);


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

    Parameters params = parse_args(argc, argv);
    ensure_csv_header(params.output_file);
    readGraphAndSolve(params.file_path, params.output_file);

    return 0;
}

Parameters parse_args(int argc, char* argv[]){
    Parameters res;
    if(argc < 2){
        std::cerr << "The file path and output path its required. \n";
        exit(1);
    }

    res.file_path = argv[1];
    res.output_file = argv[2];

    return res;
}

// Escrever no CSV se a solução é a ótima ou não
void ensure_csv_header(const std::string &filename) {
    bool file_exists = std::filesystem::exists(filename);
    std::ofstream file;
    if (!file_exists) {
        file.open(filename);
        file << "graph_name,graph_order,graph_size,density,objective_value,elapsed_time(seconds), optimal_value\n";
        file.close();
    } 
}

void write_result_to_csv(const std::string &filename, const Result &result) {
    std::ofstream file(filename, std::ios::app);
    file << result.graph_name << "," << result.node_count << ","
         << result.edge_count << "," << result.graph_density << "," << result.objValue << ","
         << result.elapsed_time << "," << result.isOptimal << "\n"; 
    file.close();
}

void readGraphAndSolve(const std::string& path, const std::string& output) {

    Result res;

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + path);
    }

    int num_vertex, num_edges;

    file >> num_vertex >> num_edges;

    res.node_count = num_vertex;
    res.edge_count = num_edges;
    res.graph_name = fs::path (path).stem().string();
    res.graph_density = static_cast<float>(2 * num_edges) / (num_vertex * (num_vertex - 1));

    Graph g;

    for(int v = 0; v < num_vertex; v++){
        g[v] = {};
    }
    
    int u, v;
    while (file >> u >> v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    file.close();
    
    // Mostra o grafo
    // for (const auto& [v, adj] : g) {
    //     std::cout << v << ": ";
    //     for (int neighbor : adj) {
    //         std::cout << neighbor << " ";
    //     }
    //     std::cout << "\n";
    
    // }
    solvePRD(g, res);

    write_result_to_csv(output, res);
}

void solvePRD(const Graph& G, Result& res) {
    try {
        // 1. Configurar o Ambiente e o Modelo
        GRBEnv env = GRBEnv(true);
        // env.set("LogFile", "optimization_log.log");
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
            // GRBQuadExpr product_xy = (1 - x_vars[v]) * (1 - y_vars[v]);

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

        // Definir tempo máximo em segundos
        model.set(GRB_DoubleParam_TimeLimit, 900);

        // 5. Otimizar o Modelo
        model.optimize();

        // 6. Exibir os Resultados
        int status = model.get(GRB_IntAttr_Status);

        // Escrever no CSV se a solução é a ótima ou não
        if (status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {
            res.objValue = model.get(GRB_DoubleAttr_ObjVal);
            res.elapsed_time = model.get(GRB_DoubleAttr_Runtime);
        }

        if(status == GRB_OPTIMAL){
            res.isOptimal = true;
        }

    } catch (GRBException e) {
        std::cout << "Erro Gurobi: " << e.getErrorCode() << " - " << e.getMessage() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Erro padrão: " << e.what() << std::endl;
    }
}