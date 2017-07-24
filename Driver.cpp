#include <iostream>
#include "Driver.h"
#include "Lex_Parse/Parser.hh"

#define YY_DECL yy::Parser::symbol_type yylex (RSWCOMP::Driver& driver)

YY_DECL;

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


int main() {

}