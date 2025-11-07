import gurobipy as gp
from gurobipy import GRB

# --- 1. Dados de Exemplo do Grafo ---
# V(G) = Conjunto de Vértices (ex: 1, 2, 3, 4, ...)
V_G = [1, 2, 3, 4]

# N(v) = Conjunto de Vizinhos de v
# Usaremos um dicionário para mapear cada vértice aos seus vizinhos
N_v = {
    1: [2, 4],
    2: [1, 3],
    3: [2, 4],
    4: [1, 3]
}
# Para um grafo maior, esses dados viriam de uma biblioteca de grafos (ex: networkx)

# --- 2. Criação do Modelo ---
try:
    # Cria o ambiente
    m = gp.Model("PLI_OtimizacaoGrafo")
    print(f"Versão do Gurobi carregada pelo Python: {gp.gurobi.version()}")

    # --- 3. Criação das Variáveis de Decisão ---
    # As variáveis são definidas sobre o conjunto de vértices V(G)
    # x_v e y_v são variáveis binárias {0, 1}

    # Variável x_v:
    x = m.addVars(V_G, vtype=GRB.BINARY, name="x")

    # Variável y_v:
    y = m.addVars(V_G, vtype=GRB.BINARY, name="y")

    # Atualiza o modelo para incluir as novas variáveis
    m.update()

    # --- 4. Definição da Função Objetivo (5.2a) ---
    # minimize Sum_{v in V(G)} (x_v + 2*y_v)

    funcao_objetivo = gp.quicksum(x[v] + 2 * y[v] for v in V_G)

    m.setObjective(funcao_objetivo, GRB.MINIMIZE)

    # --- 5. Adição das Restrições ---

    # Restrição (5.2b): para todo v em V(G)
    # 2*x_v + 2*y_v + (1 - x_v)*(1 - y_v) * Sum_{u in N(v)} 2*y_u = 2

    # OBSERVAÇÃO CRÍTICA: A restrição (5.2b) é QUADRÁTICA/NÃO-LINEAR no termo
    # (1 - x_v)*(1 - y_v) * Sum_{u in N(v)} 2*y_u.
    # Como este é um PLI, a Geração da Função Objetivo e as Restrições devem ser LINEARES.
    # O Gurobi pode lidar com restrições quadráticas se elas forem CONVEXAS,
    # mas o termo (1-x_v)(1-y_v) * ... é não-linear E não convexo.

    # O termo NÃO-LINEAR (1 - x_v)*(1 - y_v) * (2 * Sum_{u in N(v)} y_u)
    # deve ser LINEARIZADO para um PLI padrão.

    # O termo (1 - x_v)*(1 - y_v) é binário, resultando em 1 APENAS se x_v=0 E y_v=0.
    # Se x_v=1 ou y_v=1, o termo é 0.

    # Seja w_v = (1 - x_v)*(1 - y_v).
    # O produto de binárias w_v é linearizado através das seguintes restrições:
    # w_v <= 1 - x_v
    # w_v <= 1 - y_v
    # w_v >= (1 - x_v) + (1 - y_v) - 1  (i.e., w_v >= 1 - x_v - y_v)
    # w_v in {0, 1} (mas se for um produto de binárias, podemos deixar como contínua [0,1] e forçar binária)

    # Vamos linearizar:
    
    # 5.1. Criação da variável auxiliar w_v para cada v
    w = m.addVars(V_G, vtype=GRB.BINARY, name="w")

    # 5.2. Linearização do produto w_v = (1 - x_v)(1 - y_v)
    for v in V_G:
        # w_v <= 1 - x_v
        m.addConstr(w[v] + x[v] <= 1, name=f"Lin_w_{v}_x")
        # w_v <= 1 - y_v
        m.addConstr(w[v] + y[v] <= 1, name=f"Lin_w_{v}_y")
        # w_v >= 1 - x_v - y_v
        m.addConstr(w[v] >= 1 - x[v] - y[v], name=f"Lin_w_{v}_xy")

    # 5.3. Adição da Restrição (5.2b) LINEARIZADA:
    # 2*x_v + 2*y_v + w_v * Sum_{u in N(v)} 2*y_u = 2
    # O termo w_v * (Sum_{u in N(v)} 2*y_u) AINDA É NÃO-LINEAR! (Produto de binária * Soma de binárias)
    
    # Seja z_v = w_v * (Sum_{u in N(v)} 2*y_u). Este é um produto de uma variável binária (w_v)
    # e uma variável inteira/contínua (M_v = Sum_{u in N(v)} 2*y_u).

    # Para linearizar z_v = w_v * M_v (onde M_v é uma variável intermediária):
    
    # 5.4. Criação da Variável M_v (M_v = Sum_{u in N(v)} 2*y_u) e z_v
    M = m.addVars(V_G, vtype=GRB.INTEGER, name="M") # M_v é inteiro, pois é soma de 2*y_u

    # 5.5. Definição de M_v = Sum_{u in N(v)} 2*y_u
    for v in V_G:
        m.addConstr(M[v] == gp.quicksum(2 * y[u] for u in N_v[v]), name=f"Def_M_{v}")
    
    # A variável z_v = w_v * M_v, onde w_v é binária e M_v é inteira/contínua.
    # O Gurobi possui o método 'addGenConstrIndicator' ou 'addGenConstrMulti' para produtos:
    
    # Se usarmos o 'addGenConstrMulti' (Produto):
    z = m.addVars(V_G, vtype=GRB.INTEGER, name="z")
    
    for v in V_G:
        # A função addGenConstrMulti define z = w * M.
        # w_v deve ser binária e M_v deve ser contínua para o uso padrão do Gurobi Multi.
        # Como M_v é inteiro, isso é OK.
        # No Gurobi, o produto de uma variável binária (w) e uma variável limitada (M)
        # é linearizado (Big-M).
        m.addGenConstrMult(w[v], M[v], z[v], name=f"Lin_Prod_z_{v}")

    # 5.6. Adição da Restrição (5.2b) FINALMENTE LINEARIZADA:
    # 2*x_v + 2*y_v + z_v = 2
    for v in V_G:
        m.addConstr(2 * x[v] + 2 * y[v] + z[v] == 2, name=f"C52b_{v}")

    # Restrição (5.2c): x_v + y_v <= 1
    for v in V_G:
        m.addConstr(x[v] + y[v] <= 1, name=f"C52c_{v}")

    # Restrição (5.2d): x_v, y_v em {0, 1}
    # Já está definida na criação das variáveis `vtype=GRB.BINARY`.

    # --- 6. Otimização do Modelo ---
    m.optimize()

    # --- 7. Impressão dos Resultados ---
    if m.status == GRB.OPTIMAL:
        print("\n--- Solução Ótima Encontrada ---")
        print(f"Valor Ótimo da Função Objetivo: {m.objVal}")
        print("\nVariáveis x_v:")
        for v in V_G:
            if x[v].x > 0.5:
                print(f"x_{v} = {round(x[v].x)}")

        print("\nVariáveis y_v:")
        for v in V_G:
            if y[v].x > 0.5:
                print(f"y_{v} = {round(y[v].x)}")
    elif m.status == GRB.INFEASIBLE:
        print("O modelo é inviável (não tem solução).")
    elif m.status == GRB.UNBOUNDED:
        print("O modelo é ilimitado.")
    else:
        print(f"Otimização finalizada com status: {m.status}")

except gp.GurobiError as e:
    print(f'Ocorreu um erro Gurobi: {e}')
except Exception as e:
    print(f'Ocorreu um erro: {e}')