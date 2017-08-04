//
// Created by Rulon Wood on 6/12/17.
//

#include "MetaCoder.h"
namespace RSWCOMP {

    void Stop() {
        auto coder = MetaCoder::curr();
        coder->out << "li $v0, 10" << std::endl;
        coder->out << "syscall" << std::endl;
    }

    const std::shared_ptr<Expression> CharExpr(char c) {
        return std::make_shared<Expression>((int)c, CharType());
    }

    const std::shared_ptr<Expression> ChrExpr(std::shared_ptr<Expression> expr) {
        expr->intToChar();
        return expr;
    }
    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out << "\tand " << result_register->regName << ", " << e1->getRegister() << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out << "\tor " << result_register->regName << ", " << e1->getRegister() << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    void Assign(std::shared_ptr<LValue> lv, std::shared_ptr<Expression> exp) {
        if (lv->lvi != GLOBAL_REF && lv->lvi != STACK_REF) throw "Constants and Data Entries (i.e., string identifiers) Cannot Be Overwritten.";
        if (exp->containedDataType().typeName == "String" || exp->containedDataType().typeName == "") throw "Expression is either a string or null. This is unacceptable.";

        std::string destMemLoc = ""; //Destination Memory Location
        auto curr = MetaCoder::curr();

        if (lv->lvi == GLOBAL_REF) destMemLoc = std::to_string(lv->globalOffset) + "($gp)";
        else destMemLoc = std::to_string(lv->stackOffset) + "($sp)";

        curr->out << "\tsw " << exp->getRegister()->regName << "," << destMemLoc << "# This is the storage location of " << lv->name << std::endl;

    }

    std::shared_ptr<MetaCoder> MetaCoder::_content = nullptr;
    std::string MetaCoder::_outputFileName = "output.asm";

    std::shared_ptr<MetaCoder> MetaCoder::curr() {
        if(_content == nullptr) {
            std::make_shared<MetaCoder>();
            _content->out << ".globl main" << std::endl << std::endl << "main:" << std::endl;
        }
        return MetaCoder::_content;
    }

}

