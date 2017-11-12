#include <iostream>
#include "filter.h"
#include "ahdecode.h"
#include "adaptiveHuffman.h"

AHDecodeFilter::AHDecodeFilter() {
    split = false;
}

AHDecodeFilter::AHDecodeFilter(bool s) {
    split = s;
}

// Overrides the filter function to add support for the "-s" option
void AHDecodeFilter::filter(int argc, char** argv) {
    if (argc <= 1 || (split && argc == 2)) {
        processFile("-");
    } else {
        int i = split ? 2 : 1;

        for (; i != argc; i++) {
            processFile(argv[i]);
        }
    }
    std::cout.flush();
}

// The decoding filter's way of processing lines of input
// It sends an entire line of input to the model for decoding since it cannot
// know how many bits the next character is comprised of
void AHDecodeFilter::process(std::string line){
    AdaptiveHuffmanModel* model = new AdaptiveHuffmanModel(split);
    std::cout << model->decode(line) << std::endl;
    delete model;
}

// Determine whether the input characters are split into groups by spaces
// In practise, the -s makes no difference, as the decoder ignores all
// characters other than 1 and 0
int main( int argc, char** argv ) {
    bool split = false;
    if (argc > 1){
        if (std::string(argv[1]) == "-s"){
            split = true;
        }
    }

    AHDecodeFilter* filter = new AHDecodeFilter(split);
    filter->filter(argc, argv);
	std::cout.flush();
    return 0;
}
