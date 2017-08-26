//
// Created by Rulon Wood on 8/24/17.
//

#include <its_complicated/MetaCoder.h>
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

    const std::shared_ptr<Expression> Function::Call(std::string name, CallerArgs args) {
        auto curr = MetaCoder::curr();

        auto found = curr->functions.find(name);
        if (found == curr->functions.end()) throw "Function not found error: please ensure that function '" + name + "' is defined before calling.";
        if (args.numExpressions != found->second->fxSig.numArgs) throw "Call args error: please ensure your function call arguments match the expected number of arguments for the function.";

        int j = 0;
        for(auto i : args.passedArguments) {
            if (i->containedDataType().t_name != found->second->fxSig.argTypes.at(j).t_name)
                throw "Argument-definition Typename mismatch error: please ensure your function call uses the expected sequence of argument types.";
            if (i->containedDataType().memBlkSize != found->second->fxSig.argTypes.at(j).memBlkSize)
                throw "Argument-definition Typesize mismatch error: unexpected memory size encountered in argument list.";
            j++;
        }


    // Output MIPS code to prep the stack for the jump to a new function body

        // Save registers in use at the moment on the stack
        auto regsInUse = Register::usedRegisters();
        int memoryToSave = static_cast<int>((regsInUse.size()*4) + 8);

        curr->intermediateBlock << "\n\taddiu $sp, $sp, -" << memoryToSave << std::endl;
        curr->intermediateBlock << "\tsw $ra, 4($sp)" << std::endl;
        j = 0;
        for(auto i : regsInUse) {
            curr->intermediateBlock << "\tsw " << i << ", " << (j*4) + 8 << "($sp)" << std::endl;
            j++;
        }

        // Don't forget to bring along the arguments list, too
        j=0;    //Thanks, j
        for(int i = 0; i < args.numExpressions; i++) {
            j += args.passedArguments[i]->containedDataType().memBlkSize;
        }
            //Allocate space on the stack for args
        curr->intermediateBlock << "\t\taddiu $sp, $sp, -" << j << std::endl;
        for(int i = 0; i< j; i+= args.passedArguments[i]->containedDataType().memBlkSize) {
            curr->intermediateBlock << "\tsw " << args.passedArguments[i]->getRegister()->regName << "," << i <<"($sp)" << std::endl;
        }

        curr->intermediateBlock << "jal " << name << std::endl;


        //TODO: Load up all the registers once again
        curr->intermediateBlock <<"\tlw $ra, 4($sp)" << std::endl;

        j=0;
        for (auto i : regsInUse) {
            curr->intermediateBlock << "\tlw" << i << ", " << (j*4)+8 << "($sp)"<< std::endl;
            j++;
        }

        if(found->second->isProcedure) return nullptr;

        auto lvToAdd = LValue();
        lvToAdd.idString = "retVal";
        lvToAdd.type = *found->second->returnType;
        lvToAdd.lvi = STACK_REF;
        lvToAdd.stackOffset = -4;

        auto retExp = std::make_shared<Expression>(ExprFromLV(std::make_shared<LValue>(lvToAdd)));

        return retExp;
    }

    const std::shared_ptr<Expression> Function::ReturnFrom() {
        return std::shared_ptr<Expression>();
    }


}