# Compiler binary:
CPP= g++

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
	$(CPP) $(CFLAGS) $(OBJECTS) -o brkga-perfect-roman

brkga-perfect-roman.o:
	$(CPP) $(CFLAGS) -c brkga-perfect-roman.cpp

DecoderRoman.o:
	$(CPP) $(CFLAGS) -c DecoderRoman.cpp

Population.o:
	$(CPP) $(CFLAGS) -c brkgaAPI/Population.cpp

Graph.o:
	$(CPP) $(CFLAGS) -c Graph.cpp

clean:
	rm -f api-usage $(OBJECTS)
