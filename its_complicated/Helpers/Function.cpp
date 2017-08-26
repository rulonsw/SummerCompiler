//
// Created by Rulon Wood on 8/24/17.
//

#include "Function.h"

namespace RSWCOMP {
    Function::Function(FunctionSignature f, std::shared_ptr<Type> retType) {
        fxSig = f;
        returnType = retType;
    }

    void Function::Declare(std::string name, Function f) {
        auto curr = MetaCoder::curr();

        if (!curr->getScope()) throw "ScopeDef error: cannot declare functions within other functions.";

        //Looking for forward declarations
        auto found = curr->functions.find(name);
        if(found != curr->functions.end()) {
            if ((!found->second->isFwdDeclaration)
                || (found->second->isFwdDeclaration && f.isFwdDeclaration)) {
                throw "Forward declaration error: functions must be defined exactly once.";
            }

            if (found->second->fxSig.numArgs != f.fxSig.numArgs) throw "NumArgs error: functions and their definitions must have the same number of arguments.";

            int j = 0;
            for(auto i : found->second->fxSig.argTypes) {
                if(i.t_name != f.fxSig.argTypes.at(j).t_name || i.memBlkSize != f.fxSig.argTypes.at(j).memBlkSize)
                    throw "ArgTypes error: functions and their declarations must have the same sequence and type of arguments.";
                j++;
            }
            curr->functions.erase(name);
        }

        curr->functions[name] = std::make_shared<Function>(f);

        curr->mainBlockToWrite << name << ":" << std::endl;
        curr->dumpToMain();
        curr->mainBlockToWrite << "jr $ra " << std::endl;


    }

    const std::shared_ptr<Expression> Function::Call(std::string, CallerArgs) {


        return std::shared_ptr<Expression>();
    }

    const std::shared_ptr<Expression> Function::ReturnFrom() {
        return std::shared_ptr<Expression>();
    }


}