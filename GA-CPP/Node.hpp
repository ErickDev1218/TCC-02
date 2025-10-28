#ifndef NODE_HPP
#define NODE_HPP
#include <vector>

class Node {
    public:

        int label = -1;
        bool isDominated = false;
        std::vector<Node*> neighborhood;
        int dominatedFor;

        Node() = default;
        ~Node() = default;

};


#endif

