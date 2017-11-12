#ifndef AHDECODE_H
#define AHDECODE_H

#include "filter.h"

// Derived from the abstract Filter class. Overrides the process function
// to apply adaptive Huffman decoding to all input
class AHDecodeFilter: public Filter {
    private:
        bool split;

        void process(std::string);

    public:
        AHDecodeFilter();
        AHDecodeFilter(bool);

        void filter(int, char**);
};

int main(int argc, char** argv);

#endif
