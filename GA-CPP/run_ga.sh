#!/bin/bash

# Diretórios
# A array com todos os diretórios de ENTRADA
INPUT_DIRS=(
    "../Graph-base/Coleta-das-Bases/Harwell-Boeing/base_final"
    # "../Graph-base/Coleta-das-Bases/DIMACS/base_final"
    # "../Graph-base/Coleta-das-Bases/Grafo-aleatorio/base_final"
)
# O diretório RAIZ para todos os resultados
ROOT_OUTPUT_DIR="results/ga-cpp"


POP_FACTOR=1
CROSS=10
STAG=100
MAX_GENS=5
TOURNAMENT=10
ELIT=0.4
MUT=1
TRIALS=5

EXECUTABLE="./main"

# Criar o diretório RAIZ de resultados, se não existir
mkdir -p "$ROOT_OUTPUT_DIR"

# 🔄 Iniciar o loop pelos diretórios de ENTRADA
for INPUT_DIR in "${INPUT_DIRS[@]}"; do
    
    # 1. Obter o nome de subdiretório para a saída (ex: "Harwell-Boeing")

    # Base_name será usado para criar uma subpasta em ROOT_OUTPUT_DIR
    base_name=$(basename "$(dirname "$INPUT_DIR")")
    
    # 2. Definir o diretório de SAÍDA específico para a base atual
    OUTPUT_DIR="$ROOT_OUTPUT_DIR/$base_name"

    # Criar o diretório de resultados, se não existir
    mkdir -p "$OUTPUT_DIR"
    echo "========================================"
    echo "🟢 INICIANDO PROCESSAMENTO PARA A BASE: $base_name"
    echo "       Diretório de entrada: $INPUT_DIR"
    echo "       Diretório de saída: $OUTPUT_DIR"
    echo "========================================"

    # Reinicializar variáveis de estatísticas para CADA diretório
    total_graphs=0
    total_lines=0
    skipped_graphs=0
    processed_graphs=0
    start_total_time=$(date +%s)

    # 3. Criar e preencher a array de arquivos
    declare -a files=()
    while IFS= read -r graph_file; do
        num_lines=$(tr -d '\r' < "$graph_file" | wc -l)

        if ! [[ "$num_lines" =~ ^[0-9]+$ ]] || [ "$num_lines" -eq 0 ]; then
            if [ -s "$graph_file" ]; then
                num_lines=1
            else
                num_lines=0
            fi
        fi

        files+=("$num_lines:$graph_file")
    done < <(find "$INPUT_DIR" -type f -name "*.txt")


    echo "[INFO] Contagem e ordenação concluídas. Total de arquivos encontrados: ${#files[@]}"

    # Verificar se há arquivos para processar
    if [ ${#files[@]} -eq 0 ]; then
        echo "[INFO] Nenhum arquivo .txt encontrado em $INPUT_DIR. Pulando esta base."
        continue
    fi

    # Ordenar os arquivos pelo número de linhas
    IFS=$'\n' sorted_files=($(sort -n <<<"${files[*]}"))
    unset IFS

    # 4. Iniciar o loop para processar CADA arquivo dentro do diretório atual
    for entry in "${sorted_files[@]}"; do
        num_lines=${entry%%:*}
        graph_file=${entry#*:} # Caminho completo do arquivo de entrada

        # Extrair o caminho relativo AO DIRETÓRIO DE ENTRADA ATUAL
        # 🚨 CORREÇÃO CRÍTICA AQUI: Usar $INPUT_DIR (variável do loop) para extrair o caminho relativo.
        relative_path="${graph_file#$INPUT_DIR}"
        
        # O caminho relativo pode começar com uma barra, vamos garantir que a subpasta
        # de saída (output_subdir) seja construída corretamente.
        if [[ "$relative_path" == /* ]]; then
            # Se começar com barra, remove a barra inicial para a construção
            relative_path="${relative_path:1}"
        fi

        # Extrair o nome da subpasta (se existir) e do arquivo
        graph_name=$(basename "$graph_file" .txt)
        # O subdir é o diretório do arquivo em relação a $INPUT_DIR (pode ser "." se não houver subpastas)
        subdir=$(dirname "$relative_path")
        
        # 5. Criar o subdiretório de saída
        # Deve ser $OUTPUT_DIR / subpasta-relativa (se houver)
        if [[ "$subdir" == "." ]]; then
            # Se não houver subdiretório (subdir é "."), output_subdir é igual a $OUTPUT_DIR
            output_subdir="$OUTPUT_DIR"
        else
            # Caso contrário, adiciona o subdiretório relativo.
            output_subdir="$OUTPUT_DIR/$subdir"
        fi
        
        mkdir -p "$output_subdir"

        # Definir o arquivo de saída
        output_file="$output_subdir/${graph_name}.csv"

        # Verificar se o arquivo de saída já existe
        if [ -f "$output_file" ]; then
            echo "[INFO] Resultado já existe para o grafo: $graph_name na base $base_name. Pulando..."
            skipped_graphs=$((skipped_graphs + 1))
            continue
        fi

        # Atualizar estatísticas
        total_graphs=$((total_graphs + 1))
        total_lines=$((total_lines + num_lines))

        # Exibir informações sobre o grafo sendo processado
        echo "[INFO] Processando o grafo: $graph_name na base $base_name"

        # Marcar o início do tempo de execução
        start_time=$(date +%s)

        chmod +x ./main
        # Executar o algoritmo com os parâmetros especificados
        # 🚨 CORREÇÃO: Usar o EXECUTABLE e os parâmetros corretos para o seu novo algoritmo (ga-cpp)
        "$EXECUTABLE" "$graph_file" \
            --population "$POP_FACTOR" \
            --crossover "$CROSS" \
            --stagnation "$STAG" \
            --generations "$MAX_GENS" \
            --tournament "$TOURNAMENT" \
            --elitism "$ELIT" \
            --mutation "$MUT" \
            --trials "$TRIALS" \
            --output "$output_file"

        # Marcar o fim do tempo de execução
        end_time=$(date +%s)
        elapsed_time=$((end_time - start_time))

        # Exibir informações sobre o tempo de execução
        echo "[INFO] Tempo de execução para $graph_name: ${elapsed_time}s"
        echo "[INFO] Resultado salvo em: $output_file"
        echo "----------------------------------------"

        processed_graphs=$((processed_graphs + 1))
    done # Fim do loop de arquivos

    # Calcular tempo total de execução para a base atual
    end_total_time=$(date +%s)
    total_elapsed_time=$((end_total_time - start_total_time))

    # Exibir estatísticas finais para a base atual
    echo "========================================"
    echo "✅ FIM DO PROCESSAMENTO PARA A BASE: $base_name"
    echo "       Número total de grafos processados: $processed_graphs"
    echo "       Número de grafos pulados: $skipped_graphs"
    echo "       Número total de linhas processadas: $total_lines"
    echo "       Tempo total de execução da base: ${total_elapsed_time}s"
    echo "========================================"

done # Fim do loop de diretórios

echo "Execução total concluída! Resultados armazenados em: $ROOT_OUTPUT_DIR"