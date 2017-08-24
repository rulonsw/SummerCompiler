#include <iostream>
#include "Driver.h"
#include "parser.h"


    Driver::Driver()
            : trace_scanning (true), trace_parsing(true) {
    }

    int Driver::parse(const std::string& src) {

        sourceFile = src;
        begin_scan();
        yy::Parser parser (*this);
       // parser.set_debug_level(trace_parsing);
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

int main(int argc, char* argv[]) {
    Driver driver;
    //Usage of program:
        //cpslc infile [-o outfile]
    if ((argc == 3) /*if -o is used and there isn't at least one arg afterward...*/||
            (argc == 4 && argv[1] != std::string("-o"))/*if 4 args are supplied and the second isn't "-o"...*/||
            argc > 4 /*if there are more than 4 total arguments...*/) {
        std::cout << "ERR: Invalid function call. If you're attempting to invoke the compiler\n"
                "with a custom output idString, Please pair the \"-o\" option with a valid\n"
                     "number of arguments, followed by the input filename.\n"
                             "Sample:\n\n>>cpslc -o out_file_name.asm cpsl_file_name.cpsl" << std::endl;
        return -1;
    }
    try {
        int result;
        if(argc == 1) {
            driver.sourceFile = "in.cpsl";
        }
        else {
            driver.sourceFile = argv[1];

            if(argc == 4) {
                RSWCOMP::MetaCoder::_outputFileName = argv[3];
            }
        }
        result = driver.parse(driver.sourceFile);
        return result;
    } catch(std::string e) {
        std::cout << "Compilation error encountered. Error type: " << e << std::endl;
        return 1;
    }
}
