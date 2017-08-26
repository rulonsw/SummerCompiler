//
// Created by Rulon Wood on 8/24/17.
//

#ifndef SUMMERCOMPILER_FUNCTION_H
#define SUMMERCOMPILER_FUNCTION_H

#include <its_complicated/components/Type.h>
#include <vector>
#include <its_complicated/components/Expression.h>

namespace RSWCOMP {
    struct FunctionSignature {
        int numArgs;
        std::vector<Type> argTypes;
//        FunctionSignature & operator= (const FunctionSignature &rhs) {
//            argTypes = rhs.argTypes;
//            numArgs = rhs.numArgs;
//
//        }
    };
    struct CallerArgs {
        int numExpressions;
        std::vector<std::shared_ptr<Expression>> passedArguments;
    };

    struct Function {
        bool isProcedure;
        bool isFwdDeclaration;

        std::shared_ptr<Type> returnType;
        FunctionSignature fxSig;

        Function(FunctionSignature, std::shared_ptr<Type>);

        void Declare(std::string, Function);
        void loadLocalVariables(CallerArgs args);
        const std::shared_ptr<Expression> Call(std::string, CallerArgs);
        const std::shared_ptr<Expression> ReturnFrom();
    };
}



#endif //SUMMERCOMPILER_FUNCTION_H
