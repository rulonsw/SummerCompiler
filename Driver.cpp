#include <iostream>
#include "Driver.h"
#include "parser.hpp"



namespace RSWCOMP {

    Driver::Driver()
            : trace(false) {

    }

    int Driver::parse(const std::string& src) {
        sourceFile = src;
        begin_scan();

    }

    void Driver::begin_scan() {

    }

    void Driver::end_scan() {

    }
}

int main(int argc, char* argv[]) {
    //Usage of program:
        //cpslc [-o outfile] infile
    if ((argc == 3 && argv[1] == std::string("-o")) /*if -o is used and there aren't at least two args afterward...*/||
            (argc == 4 && argv[1] != std::string("-o"))/*if 4 args are supplied and the second isn't "-o"...*/||
            argc > 4 /*if there are more than 4 total arguments...*/) {
        std::cout << "ERR: Invalid function call. If you're attempting to invoke the compiler\n"
                "with a custom output name, Please pair the \"-o\" option with a valid\n"
                     "number of arguments, followed by the input filename.\n"
                             "Sample:\n\n>>cpslc -o out_file_name cpsl_file_name" << std::endl;
        return -1;
    }
}