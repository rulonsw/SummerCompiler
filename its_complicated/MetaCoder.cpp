//
// Created by Rulon Wood on 6/12/17.
//

#include "MetaCoder.h"
namespace RSWCOMP {

    std::shared_ptr<LValue> LVFromID(std::string yo) {
        auto curr = MetaCoder::curr();
        auto findResult = find(curr->LVs.begin(), curr->LVs.end(), yo);
        if (findResult == curr->LVs.end()) {
            throw "LValue with id " + yo + " not found.";
        }
        return findResult->second;
    }

    std::shared_ptr<Expression> ExprFromLV(std::shared_ptr<LValue> lv) {
        auto curr = MetaCoder::curr();

        auto result = find(curr->LVs.begin(), curr->LVs.end(), lv->name);
        if(result == curr->LVs.end()) throw "LValue " + lv->name + " not found in current MetaCoder map.";

        auto tempLV = result->second;

        Expression expr;
        switch (tempLV->lvi) {
            case GLOBAL_REF:
                expr = Expression(Register::consumeRegister(), tempLV->type);
                curr->out << "\tlw " << expr.getRegister()->regName <<", " << tempLV->globalOffset << "($gp)" << std::endl;
                return std::make_shared<Expression>(expr);
            case STACK_REF:
                expr = Expression(Register::consumeRegister(), tempLV->type);
                curr->out << "\tlw " << expr.getRegister()->regName <<", " << tempLV->stackOffset << "($sp)" << std::endl;
                return std::make_shared<Expression>(expr);
            case DATA:
                expr = Expression(Register::consumeRegister(), tempLV->type);
                curr->out << "\tla " << expr.getRegister()->regName <<", " << tempLV->name << std::endl;
                return std::make_shared<Expression>(expr);
            case CONST:
                expr = Expression(tempLV->constVal, tempLV->type);
                return std::make_shared<Expression>(expr);
            default:
                throw "Unknown LV type present in tempLV. LVI code " + tempLV->lvi ;
        }
    }

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
    std::string StringToAsciizData(std::shared_ptr<Expression> exp) {
        if (exp->containedDataType().typeName != "String") {
            throw "attempted to label non-string data type. Type: " + exp->containedDataType().typeName;
        }
        auto curr = MetaCoder::curr();
        std::string stringLabel = "stringLabel" + curr->nextStringCtr();
        return stringLabel;
    }

    void declareConst(std::string id, std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        if (find(curr->existingIds.begin(), curr->existingIds.end(), id) != curr->existingIds.end()) {
            throw "data with existing id " + id + " is already being used.";
        }
        LValue newLV;
        newLV.name = id;
        newLV.type = exp->containedDataType();

        if (exp->containedDataType().typeName == "String") {
            newLV.lvi = DATA;
            newLV.name = id;
            curr->constExprs[id] = exp;

        }
        else {
            newLV.lvi = GLOBAL_REF;
            newLV.globalOffset = curr->topOfGlobal();
            curr->out << "\tsw " << exp->getRegister()->regName << ", " << newLV.globalOffset <<"($gp) #id: "<< id << std::endl;
        }
        curr->LVs[id] = std::make_shared<LValue>(newLV);
    }

    void ReadValue(std::shared_ptr<LValue> lv) {
        if (lv->lvi == DATA || lv->lvi == CONST) throw "Can't read into constant memory locations.";
        auto curr = MetaCoder::curr();

        int MIPSReadType = 0;

        if (lv->type.typeName == "Integer") MIPSReadType = 5;
        else if (lv->type.typeName == "Character") MIPSReadType = 12;

        std::string destMemLoc = "";
        if (lv->lvi == GLOBAL_REF) destMemLoc = lv->globalOffset + "($gp)";
        else if (lv->lvi == STACK_REF) destMemLoc = lv->stackOffset + "($sp)";

        curr->out << "\tli $v0, " << MIPSReadType << "\n\tsyscall\n\tsw $v0," << destMemLoc << std::endl;
    }

    void WriteExpr(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        switch(exp->containedDataType().memBlkSize) {
            case 4:
                if(exp->containedDataType().typeName == "Integer" || exp->containedDataType().typeName == "Boolean") {
                    curr->out << "\tli $v0, 1\n\tli $a0, " << exp->getRegister()->regName << "\n\tsyscall\n";
                }
                else {
                    curr->out << "\tli $v0, 11\n\tli $a0, " << exp->getRegister()->regName << "\n\tsyscall\n";
                }
                break;
            case -1: //This means it's a string. Ain't no way it'll be nothin' else
                std::string searchFor = StringToAsciizData(exp);
                declareConst(searchFor,std::make_shared<Expression>(exp->getStrVal()));

                std::shared_ptr<LValue> lvTemp = LVFromID(searchFor);
                auto expRet = ExprFromLV(lvTemp);

                curr->out << "\tli $v0, 4\n\tla $a0, " << expRet->getRegister()->regName << "\n\tsyscall\n";
                break;
            default:
                throw "Something horrible has happened. Your expression holds a non-standard data type, and is thus unprintable.";

        }


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

