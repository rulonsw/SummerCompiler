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
        if (exprId == "UNSET_ID" || curr->constExprs.find(exprId) == curr->constExprs.end()) {
            exprType = memoryLocation;
            curr->mainBlockToWrite << "\tli " << consumed_reg->regName << "," << numericValue << std::endl;

            numericValue = INT32_MIN;
            return consumed_reg;
        }
        else {
            exprType = memoryLocation;
            if(containsDataType.t_name == T_STRING) {
                curr->mainBlockToWrite << "\tmove " << consumed_reg->regName << ", " << exprId << std::endl;
            }
            else {
                curr->mainBlockToWrite << "\tlw " << consumed_reg->regName << "," << exprId << std::endl;
            }
            return consumed_reg;
        }

    }

    void Expression::intToChar() {
        if(containsDataType.t_name == T_INTEGER || containsDataType.t_name == T_CHARACTER) {
            containsDataType.t_name = T_CHARACTER;
        }
        else {
            std::cout << "Incompatible type - only integers are castable to characters." << std::endl;
            throw "Typecast error - intToChar";

        }
    }
    void Expression::charToInt() {
        if(containsDataType.t_name == T_INTEGER || this->containsDataType.t_name == T_CHARACTER) {
            containsDataType.t_name = T_INTEGER;
        }
        else {
            std::cout << "Incompatible type - only characters are castable to integers." << std::endl;
            throw "Typecast Error - charToInt";
        }
    }

    void Expression::step(bool up) {
        switch(containsDataType.t_name) {
            case T_INTEGER:
            case T_CHARACTER:
                numericValue = up ? numericValue + 1 : numericValue - 1;
                break;
            case T_BOOLEAN:
                numericValue = !numericValue;
                break;
            default:
                std::cout << "Method incompatible with data type - only boolean values, characters and integers are step-compatible." << std::endl;
                throw "Invalid Use of Member Method - step(bool up)";
        }

    }
}
