#include <iostream>
#include "Driver.h"




    Driver::Driver()
            : trace(false) {
    }

    int Driver::parse(const std::string& src) {

        sourceFile = src;
        begin_scan();
        yy::Parser parser (*this);
        int p_result = parser.parse();
        end_scan();
        return p_result;
    }
    Driver::~Driver() {}
    void Driver::error (const yy::location& l, const std::string& m)
    {
        std::cerr << l << ": " << m << std::endl;
    }

    void Driver::error (const std::string& m)
    {
        std::cerr << m << std::endl;
    }
    void Driver::begin_scan() {

    }

    void Driver::end_scan() {

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
    try {

    }catch(std::string e){
        std::cout << "Compilation error encountered. Error type: " << e << std::endl;
        return 1;
    }
}