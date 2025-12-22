# Compiler binary:
UNAME_S:=$(shell uname -s)

ifeq ($(UNAME_S), Darwin)
	CXX=g++-15
else
	CXX=g++
endif


# Recommended compiler flags for speed:
#	OpenMP enabled
#	full binary code optimization
#	full error and warning reports
#	no range checking within BRKGA:
CFLAGS= -std=c++17 -O3 -fopenmp -Wextra -Wall -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Compiler flags for debugging; uncomment if needed:
#	range checking enabled in the BRKGA API
#	OpenMP disabled
#	no binary code optimization
#CFLAGS= -DRANGECHECK -Wextra -Wall -Weffc++ -ansi -pedantic -Woverloaded-virtual -Wcast-align -Wpointer-arith

# Objects:
OBJECTS= Population.o DecoderRoman.o Graph.o brkga-perfect-roman.o

# Targets:
all: brkga-perfect-roman

brkga-perfect-roman: $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o brkga-perfect-roman

brkga-perfect-roman.o:
	$(CXX) $(CFLAGS) -c brkga-perfect-roman.cpp

DecoderRoman.o:
	$(CXX) $(CFLAGS) -c DecoderRoman.cpp

Population.o:
	$(CXX) $(CFLAGS) -c brkgaAPI/Population.cpp

Graph.o:
	$(CXX) $(CFLAGS) -c Graph.cpp

clean:
	rm -f api-usage $(OBJECTS)
