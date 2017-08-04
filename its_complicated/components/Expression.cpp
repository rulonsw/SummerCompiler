//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include "Expression.h"
#include "../MetaCoder.h"
#include "../../Register.h"

namespace RSWCOMP {

    std::shared_ptr<Register> Expression::getRegister() {
        if(this->exprType == memoryLocation) {
            return regLocation;
        }
        if(this->exprType == strHolder) {
            throw "Cannot perform 'li' operation on string value";
        }
        auto curr = MetaCoder::curr();

        auto consumed_reg = Register::consumeRegister();
        exprType = memoryLocation;
        curr->out << "\tli " << consumed_reg->regName << "," << numericValue << std::endl;

        numericValue = INT32_MIN;
        return consumed_reg;
    }

    void Expression::intToChar() {
        if(this->containsDataType.typeName == "Integer" || this->containsDataType.typeName == "Character") {
            this->containsDataType.typeName = "Character";
        }
        else {
            std::cout << "Incompatible type - only integers are castable to characters." << std::endl;
            throw "Typecast error - intToChar";

        }
    }
    void Expression::charToInt() {
        if(this->containsDataType.typeName == std::string("Integer") || this->containsDataType.typeName == std::string("Character")) {
            this->containsDataType.typeName = "Integer";
        }
        else {
            std::cout << "Incompatible type - only characters are castable to integers." << std::endl;
            throw "Typecast Error - charToInt";
        }
    }
}
