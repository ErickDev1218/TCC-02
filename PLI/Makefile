# Compilador
CXX = g++

# Diretórios do Gurobi
GUROBI_HOME ?= $(GUROBI_HOME)
INCLUDE = -I$(GUROBI_HOME)/include
LIB = -L$(GUROBI_HOME)/lib

# Flags de compilação
CXXFLAGS = $(INCLUDE)
LDFLAGS = $(LIB) -lgurobi_c++ -lgurobi110

# Nome do executável
TARGET = main

# Arquivo fonte
SRC = main.cpp

# Regra padrão
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# Limpar arquivos gerados
clean:
	rm -f $(TARGET)

# Executar o programa
run: $(TARGET)
	./$(TARGET)
