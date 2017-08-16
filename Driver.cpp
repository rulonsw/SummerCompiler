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
        //cpslc [-o outfile] infile
    if ((argc == 3) /*if -o is used and there aren't at least two args afterward...*/||
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
//        if(argc == 1) {
            result = driver.parse("/Users/rulonwood/CLionProjects/SummerCompiler/in.cpsl");
//        }
//        else {
//            result = argc == 2? driver.parse(argv[1]) : driver.parse(argv[3]);
//            std::cout << argc << std::endl;
//            std::cout << argv[0] << "     " << argv[1] << "     " << argv[2] << "     " << argv[3] << std::endl;
//        }


        return result;

    }catch(std::string e){
        std::cout << "Compilation error encountered. Error type: " << e << std::endl;
        return 1;
    }
}
