# Compiler binary:
CPP= g++

# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:
CFLAGS= -std=c++17 -O3 -fopenmp -Wextra -Wall -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

TARGET := main

# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith
SOURCES := $(wildcard *.cpp)
# Objects:
OBJECTS=$(SOURCES:.cpp=.o)

# Gera arquivos de dependência (.d) automaticamente
DEPS := $(OBJECTS:.o=.d)

# Targets:
.PHONY: all
all: $(TARGET)

# Regra de linkagem — cria o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

# Regra genérica para compilar .cpp em .o + gerar dependências
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Inclui dependências se existirem (faz recompilar se headers mudarem)
-include $(DEPS)

# Limpeza
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

# Limpeza total (inclusive backups, se houver)
.PHONY: distclean
distclean: clean
	rm -f *~