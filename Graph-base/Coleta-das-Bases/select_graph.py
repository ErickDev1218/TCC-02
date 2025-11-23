import os
import shutil
from pathlib import Path
import math

def ordenar_e_copiar_txt(input_dir, output_dir):
    input_path = Path(input_dir)
    output_path = Path(output_dir)

    # Garante que o diretório de saída exista
    output_path.mkdir(parents=True, exist_ok=True)

    # Lista todos os arquivos .txt
    arquivos_txt = list(input_path.glob("*.txt"))

    # Cria uma lista de tuplas: (arquivo, quantidade_de_linhas)
    arquivos_com_linhas = []
    for arquivo in arquivos_txt:
        with open(arquivo, "r", encoding="utf-8") as f:
            num_linhas = sum(1 for _ in f)
        arquivos_com_linhas.append((arquivo, num_linhas))

    # Ordena pela quantidade de linhas
    arquivos_ordenados = sorted(arquivos_com_linhas, key=lambda x: x[1], reverse=True)
    
    groups = len(arquivos_ordenados) // 3 # Divisao inteira -> 150 = 50 50 50 ou 100 = 33 33 33

    amount = math.ceil(groups * 0.3) # 30% de cada grupo -> 50 * 0.3 = 15 ou 33 * 0.3 = 9.9 = 10

    slices = []

    for i in range(3):
        init = i * groups
        final = init + groups

        slice_start = final - amount
        slice_end = final

        slices.append(arquivos_ordenados[slice_start : slice_end])


    # Copia para o diretório de saída
    for slice in slices:
        for arquivo, _ in slice:
            destino = output_path / arquivo.name
            shutil.copy2(arquivo, destino)
            print(f"Copiado: {arquivo.name})")

    print("\nProcesso concluído!")



if __name__ == "__main__":

    BASE_PATH = Path(__file__).parent   

    GRAPH_PATH = [
        (f"{BASE_PATH}/Grafo-aleatorio/base_final/", f"{BASE_PATH}/IRACE_graphs/"),
        (f"{BASE_PATH}/DIMACS/base_final/", f"{BASE_PATH}/IRACE_graphs/"),
        (f"{BASE_PATH}/Harwell-Boeing/base_final/", f"{BASE_PATH}/IRACE_graphs/")
    ]

    for input, output in GRAPH_PATH:

        ordenar_e_copiar_txt(
            input_dir=input,
            output_dir=output,
        )