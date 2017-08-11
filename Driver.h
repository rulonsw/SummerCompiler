//
// Created by Rulon Wood on 5/29/17.
//

#ifndef SUMMERCOMPILER_DRIVER_H
#define SUMMERCOMPILER_DRIVER_H
#include<map>
#include<string>
#include "parser.h"

# define YY_DECL \
  yy::Parser::symbol_type yylex (Driver& driver)
// ... and declare it for the parser's sake.
YY_DECL;
// This is the actual driver of the compiler. This will control input flow, among other things.
// Things that are included here:
//   - References to the file being opened
//   - Read/Write control
//   - Result value
//   - Name of the file to be opened
//   - Map of variables within scope


    class Driver {
    public:
        //Con/Destructors
        Driver();

        virtual ~Driver();

        //Class variables
        std::map<std::string, int> vars;
        std::string sourceFile;
        int parse_result;
        bool trace_scanning;
        bool trace_parsing;

        //Actual parsing method
        int parse(const std::string &sourceFile);

        // Abstract function control
        void begin_scan();

        void end_scan();

        void error(const yy::location &l, const std::string &m);

        void error(const std::string &m);


    };


#endif //SUMMERCOMPILER_DRIVER_H
