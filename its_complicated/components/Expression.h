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
        int numericValue = INT32_MAX;
        std::string strValue = "DEFAULT_UNSET";

    public:
        inline bool operator==(Expression& rhs) {
            return this->getNumericValue() == rhs.getNumericValue()
                    && this->getStrVal() == rhs.getStrVal();
        }
        std::string exprId = "UNSET_ID";
        void intToChar();
        void charToInt();

        void step(bool up);

        std::string getStrVal() {return strValue;}
        int getNumericValue() {return numericValue;}

        std::shared_ptr<Register> getRegister();
        Type containedDataType() {
            return containsDataType;
        }

        Expression(std::shared_ptr<Register> reg, Type type) {
            containsDataType = type;
            regLocation = reg;
            exprType = memoryLocation;
            numericValue = INT32_MIN;

        }

        Expression(int intVal, Type t) {
            numericValue = intVal;
            exprType = intgr;
            containsDataType = t;
        }

        Expression(std::string str, Type t) {
            numericValue = INT32_MIN;
            strValue = str;
            exprType = strHolder;
            containsDataType = t;
        }

    };
}



#endif //SUMMERCOMPILER_EXPRESSION_H
