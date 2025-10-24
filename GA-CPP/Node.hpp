#ifndef NODE_HPP
#define NODE_HPP
#include <vector>

class Node {
    public:

        int label = -1;
        bool isDominated = false;
        std::vector<Node*> neighborhood;
        std::vector<bool> dominanceNumber;

        Node() = default;
        ~Node() = default;

};


#endif
