#include <iostream>
#include <fstream>
#include "filter.h"

// Iterates through the lines in an input stream, processing them in whatever
// way the class instance's process function is defined
void Filter::processStream(std::istream& stream){
    std::string line;
    while (getline(stream, line)){
        process(line);
    }
}

// Attempts to open an input stream for stdin or the specified files
void Filter::processFile(std::string const& filename) {
    if (filename == "-") {
        processStream(std::cin);
    } else {
        std::ifstream in(filename.c_str());
        if (!in.is_open()) {
            std::cerr << "Error: cannot open " << filename << std::endl;
        } else {
            processStream(in);
        }
    }
}

// Determines whether to process stdin or a list of files supplied on the
// command line
void Filter::filter(int argc, char** argv) {
    if (argc == 1) {
        processFile("-");
    } else {
        for (int i = 1; i != argc; i++) {
            processFile(argv[i]);
        }
    }
    std::cout.flush();
}
