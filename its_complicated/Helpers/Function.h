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
        // As per CPSL Document: "The signature consists of the
        //    name of the function as well as the number, type, and order of the parameters."

        std::string name;
        int numArgs;
        std::vector<Type> argTypes;
        //argNames only used during local variable assignment
        std::vector<std::string> argNames;

        FunctionSignature(std::string, int, std::vector<Type>);
        FunctionSignature();
        FunctionSignature(std::vector<std::string>, std::vector<Type>);
    };
    struct CallerArgs {
        int numExpressions;
        std::vector<std::shared_ptr<Expression>> passedArguments;
    };

    struct Function {
        bool isProcedure;
        bool isFwdDeclaration;

        Type returnType;
        FunctionSignature fxSig;

        Function(FunctionSignature, Type);
        Function();

        void Declare(std::string, Function);
        void loadLocalVariables(std::string, FunctionSignature args);
        const std::shared_ptr<Expression> Call(std::string, CallerArgs);
        const std::shared_ptr<Expression> ReturnFrom(std::string name);
    };
}



#endif //SUMMERCOMPILER_FUNCTION_H
