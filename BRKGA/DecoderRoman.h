#ifndef DECODER_ROMAN_H
#define DECODER_ROMAN_H

#include <list>
#include <vector>
#include <algorithm>
#include <random>
#include "brkgaAPI/MTRand.h"
#include "Graph.h"

class DecoderRoman {
public:
    // Constructor
	DecoderRoman(const Graph& graph) : g{graph} { }	

    // Destructor
	~DecoderRoman() = default;	        

	// Decode a chromosome, returning its fitness as a double-precision floating point:
    double decode(std::vector< double >& chromosome) const;

private:
	const Graph& g;
};

#endif