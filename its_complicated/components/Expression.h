//
// Created by Rulon Wood on 6/12/17.
//

#ifndef SUMMERCOMPILER_EXPRESSION_H
#define SUMMERCOMPILER_EXPRESSION_H

#include <Register.h>
#include "Type.h"

namespace RSWCOMP {
    enum ExprDataType {
        memoryLocation,
        intgr,
        strHolder
    };


    class Expression {
    private:
        ExprDataType exprType;
        std::shared_ptr<RSWCOMP::Register> regLocation = nullptr;
        RSWCOMP::Type containsDataType;
        int numericValue = 42;
        std::string strValue = "DEFAULT_UNSET";

    public:

        Expression(std::shared_ptr<Register> reg, Type type) {
            containsDataType = type;
            regLocation = reg;
            exprType = memoryLocation;
            numericValue = -2147483648;

        }

        Expression(int intVal, Type t) {
            numericValue = intVal;
            exprType = intgr;
            containsDataType = t;
        }

        Expression(std::string str, Type t) {
            numericValue = -2147483648;
            strValue = str;
            exprType = strHolder;
            containsDataType = t;
        }

    };
}



#endif //SUMMERCOMPILER_EXPRESSION_H
