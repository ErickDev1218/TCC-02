import os
import csv

# pasta onde estão os arquivos .txt
input_folder = "/home/fox/GitHub-Temporario/TCC-02/Graph-base/Coleta-das-Bases/DIMACS/base_final"

output_csv = "resumo_grafos_dimacs.csv"

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

# escreve o CSV
with open(output_csv, "w", newline="") as csvfile:
    fieldnames = ["arquivo", "n_vertices", "n_arestas", "densidade"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()
    for row in results:
        writer.writerow(row)

print(f"Arquivo '{output_csv}' criado com sucesso.")
