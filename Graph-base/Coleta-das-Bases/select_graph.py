import os
import shutil
from pathlib import Path
import math
import random

def ordenar_e_copiar_txt(input_dir, output_dir):
    input_path = Path(input_dir)
    output_path = Path(output_dir)

    # Garante que o diretório de saída exista
    output_path.mkdir(parents=True, exist_ok=True)

    # Lista todos os arquivos .txt
    arquivos_txt = list(input_path.glob("*.txt"))

    # Número total e quantidade a copiar (30%)
    total = len(arquivos_txt)
    amount = math.ceil(total * 0.3)  # 30% do total REAL

    # Cria lista (arquivo, linhas)
    arquivos_com_linhas = []
    for arquivo in arquivos_txt:
        with open(arquivo, "r", encoding="utf-8") as f:
            num_linhas = sum(1 for _ in f)
        arquivos_com_linhas.append((arquivo, num_linhas))

    # Ordena pela quantidade de linhas (desc)
    arquivos_shuffle = arquivos_com_linhas.copy()
    random.shuffle(arquivos_shuffle)
    # arquivos_ordenados = sorted(arquivos_com_linhas, key=lambda x: x[1], reverse=True)

    # Seleciona os top 30%
    selecionados = arquivos_shuffle[:amount]

    # Copia arquivos
    for arquivo, _ in selecionados:
        destino = output_path / arquivo.name
        shutil.copy2(arquivo, destino)
        print(f"Copiado: {arquivo.name}")

    print(f"\nProcesso concluído! Copiados {len(selecionados)} de {total} arquivos.\n")




if __name__ == "__main__":

    BASE_PATH = Path(__file__).parent   

    GRAPH_PATH = [
        (f"{BASE_PATH}/DIMACS/base_final/", f"{BASE_PATH}/IRACE_graphs/"),
        (f"{BASE_PATH}/Harwell-Boeing/base_final/", f"{BASE_PATH}/IRACE_graphs/"),
        (f"{BASE_PATH}/Random_graphs/base_final/", f"{BASE_PATH}/IRACE_graphs/")
    ]

    for input, output in GRAPH_PATH:

        ordenar_e_copiar_txt(
            input_dir=input,
            output_dir=output,
        )