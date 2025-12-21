import os
import csv
import pandas as pd
import numpy as np

# pasta onde estão os arquivos .txt
# input_folder = "/Users/erickdev1218/Desktop/Repositorios/TCC-02/Graph-base/Coleta-das-Bases/DIMACS/base_final"

input_folder = "/Users/erickdev1218/Desktop/Repositorios/TCC-02/Graph-base/Coleta-das-Bases/Harwell-Boeing/base_final"

# output_csv = "/Users/erickdev1218/Desktop/Repositorios/TCC-02/Graph-base/Coleta-das-Bases/Harwell-Boeing/resumo_grafos_dimacs.csv"

results = []

for filename in os.listdir(input_folder):
    if filename.endswith(".txt"):
        path = os.path.join(input_folder, filename)

        with open(path, "r") as f:
            first_line = f.readline().strip()
            if not first_line:
                continue

            n, m = map(int, first_line.split())

            # densidade para grafo simples não direcionado
            if n > 1:
                density = 2 * m / (n * (n - 1))
            else:
                density = 0.0

            results.append({
                "arquivo": filename,
                "n_vertices": n,
                "n_arestas": m,
                "densidade": density
            })

results_sorted = sorted(
    results,
    key=lambda r: r['densidade'],
)

results_filtered = [
    r for r in results_sorted
    if r["n_vertices"] < 1400 and r['n_arestas'] > 0
]

df = pd.DataFrame(results_filtered)
# bins de tamanho
df["size_bin"] = pd.qcut(
    df["n_vertices"],
    q=3,
    labels=["small", "medium", "large"]
)

# bins de densidade
df["density_bin"] = pd.qcut(
    df["densidade"],
    q=3,
    labels=["low", "medium", "high"]
)

# estrato combinado
df["stratum"] = df["size_bin"].astype(str) + "_" + df["density_bin"].astype(str)

N = 75

sampled = (
    df
    .groupby("stratum", group_keys=False)
    .apply(
        lambda x: x.sample(
            max(1, int(len(x) / len(df) * N)),
            random_state=42
        )
    )
)

if len(sampled) < N:
    remaining = df.drop(sampled.index)
    extra_needed = N - len(sampled)
    if len(remaining) >= extra_needed:
        extra = remaining.sample(extra_needed, random_state=42)
        sampled = pd.concat([sampled, extra])
    else:
        # se não há elementos suficientes, pega todos disponíveis
        sampled = pd.concat([sampled, remaining])


# Ajuste fino (se passar de 75)
# sampled = sampled.sample(N, random_state=42)

selected_graphs = sampled.to_dict(orient="records")

results_sorted = sorted(
    selected_graphs,
    key=lambda r: r['densidade'],
)

names = [r["arquivo"] for r in results_sorted]

# print(names)

selected_basenames = {os.path.splitext(n)[0] for n in names}
MOVE_INSTEAD_OF_DELETE = True

where_remove = "/Users/erickdev1218/Desktop/Repositorios/TCC-02/Graph-base/Coleta-das-Bases/Harwell-Boeing/descompactados"

removed_folder = os.path.join(where_remove, "removed") if MOVE_INSTEAD_OF_DELETE else None

if MOVE_INSTEAD_OF_DELETE:
    os.makedirs(removed_folder, exist_ok=True)

for filename in os.listdir(where_remove):
    if not filename.endswith(".mtx"):
        continue
    # compara pelo nome base (sem extensão)
    if os.path.splitext(filename)[0] in selected_basenames:
        continue
    src = os.path.join(where_remove, filename)
    try:
        if MOVE_INSTEAD_OF_DELETE:
            dst = os.path.join(removed_folder, filename)
            os.replace(src, dst)
            print(f"Moved: {filename}")
        else:
            os.remove(src)
            print(f"Deleted: {filename}")
    except Exception as e:
        print(f"Failed to remove {filename}: {e}")


# # escreve o CSV
# with open(output_csv, "w", newline="") as csvfile:
#     fieldnames = ["arquivo", "n_vertices", "n_arestas", "densidade"]
#     writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

#     writer.writeheader()
#     for row in results:
#         writer.writerow(row)

# print(f"Arquivo '{output_csv}' criado com sucesso.")
