#ifndef NODE_HPP
#define NODE_HPP
#include <vector>

class Node {
    public:

        int label = -1;
        bool isDominated = false;
        std::vector<Node*> neighborhood;
        int dominatedFor = 0;

        Node() = default;
        ~Node() = default;

        void checkSelfDominance(){
            int n = this->dominatedFor;
            if(n == 0){
                this->isDominated = false;
            }else if(n == 1 || n == 2){
                this->isDominated = true;
            }
        }


};


#endif


