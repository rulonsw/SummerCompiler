//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include <algorithm>
#include "MetaCoder.h"
namespace RSWCOMP {
/****Sections****/
    void MainBlock() {
        ConstBlock();
        auto curr = MetaCoder::curr();
        Stop();
    }

    //TODO: Debug this method below, in specific.
    //TODO: Problem -
    void ConstBlock() {
        auto curr = MetaCoder::curr();
        curr->out << ".data" << std::endl;

        for (auto foo : curr->constExprs) {
            if (foo.second->containedDataType().t_name == T_STRING) {
                curr->out << foo.first << " .asciiz " << "\"" << foo.second->getStrVal() << "\"" << std::endl;
            }
            else {
                curr->out << foo.first << " .word " << foo.second->getNumericValue() << std::endl;
            }
        }
        curr->out << "\n\n\n";
    }

    MetaCoder::MetaCoder() {
        std::cout << "Writing code to " << _outputFileName << "..." << std::endl;
        out.open(_outputFileName);
    }
    MetaCoder::~MetaCoder() {
        out.close();
    }
/*****LVALUE METHODS*****/
    std::shared_ptr<LValue> LVFromID(std::string s) {
        auto curr = MetaCoder::curr();
        auto findResult = curr->LVs.find(s);
        if (findResult == curr->LVs.end()) {
            throw "LValue with id " + s + " not found.";
        }
        return findResult->second;
    }

    std::shared_ptr<Expression> ExprFromLV(std::shared_ptr<LValue> lv) {
        auto curr = MetaCoder::curr();

        auto result = curr->LVs.find(lv->cpsl_refname);
        if(result == curr->LVs.end()) throw "LValue " + lv->cpsl_refname + " not found in current MetaCoder map.";

        auto tempLV = result->second;

        Expression expr("", StringType());
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
                curr->out << "\tla " << expr.getRegister()->regName <<", " << tempLV->idString << std::endl;
                return std::make_shared<Expression>(expr);
            case CONST:
                expr = Expression(tempLV->constVal, tempLV->type);
                return std::make_shared<Expression>(expr);
            default:
                throw "Unknown LV type present in tempLV." ;
        }
    }

/*****BASIC CONTROL FLOW*****/
    void Stop() {
        auto coder = MetaCoder::curr();
        coder->out << "li $v0, 10" << std::endl;
        coder->out << "syscall" << std::endl;
    }

/*****CASTING/VALUE OPERATIONS*****/
    const std::shared_ptr<Expression> CharExpr(char c) {
        return std::make_shared<Expression>((int)c, CharType());
    }

    const std::shared_ptr<Expression> IntExpr(int i) {
        return std::make_shared<Expression>(i, IntType());
    }

    const std::shared_ptr<Expression> StringExpr(std::string s) {
        return std::make_shared<Expression>(s, StringType());
    }

    void MakeId(std::string s) {
        auto curr = MetaCoder::curr();
        curr->ids_toWrite.push_back(s);
    }
    void MakeVar(Type t) {
        /*This method is essentially only called when there's 1+ things in the `ids_toWrite` vector.*/
        auto curr = MetaCoder::curr();

        if(curr->ids_toWrite.size() == 0) throw("Variable creation attempt with empty list of var names.");

        while(curr->ids_toWrite.size() != 0) {
            auto foo = curr->ids_toWrite[0];
            auto isFound = curr->LVs.find(foo);
            if (isFound != curr->LVs.end()) {
                throw("Duplicate LValue idString found during compilation. Please retry using distinct var names for each variable.");
            }
            LValue lv;

            lv.lvi = GLOBAL_REF;
            lv.idString = foo;
            lv.globalOffset = curr->topOfGlobal(t.memBlkSize);
            lv.type = t;
            curr->LVs[foo] = std::make_shared<RSWCOMP::LValue>(lv);

            curr->ids_toWrite.erase(curr->ids_toWrite.begin());
        }

    }

    const std::shared_ptr<Expression> ChrExpr(std::shared_ptr<Expression> expr) {
        expr->intToChar();
        return expr;
    }
    const std::shared_ptr<Expression> OrdExpr(std::shared_ptr<Expression> e) {
        e->charToInt();
        return e;
    }

    const std::shared_ptr<Expression> PredExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();

        if(e->containedDataType().t_name == T_CHARACTER || T_INTEGER) {
            curr->out << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", -1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->out << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(false);
        return e;

    }
    const std::shared_ptr<Expression> SuccExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();

        if(e->containedDataType().t_name == T_CHARACTER || T_INTEGER) {
            curr->out << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", 1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->out << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(true);
        return e;
    }

/*****BOOLEAN OPERATIONS*****/
    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out << "\tand " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out << "\tor " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NotExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tnot " << result_register->regName << "," << e->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

/*****MATH OPERATIONS*****/
    const std::shared_ptr<Expression> AddExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out <<"\tadd " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> SubExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out <<"\tsub " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> MultExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out <<"\tmul " <<result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> DivExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->out <<"\tdiv " <<result_register->regName << "," << e1->getRegister() << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> ModExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tdiv " << e1->getRegister()->regName << "," << e2->getRegister()-> regName << "\n\tmfhi " << result_register->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> UnMinusExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tneg " << result_register->regName << "," << e->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

    const std::shared_ptr<Expression> EqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tseq " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NeqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tsne " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> GteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tsge " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> GtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tsgt " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tsle " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->out << "\tslt " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }


/*****DATA OPERATIONS*****/
    void Assign(std::shared_ptr<LValue> lv, std::shared_ptr<Expression> exp) {
        if (lv->lvi != GLOBAL_REF && lv->lvi != STACK_REF) throw "Constants and Data Entries (i.e., string identifiers) Cannot Be Overwritten.";
        if (exp->containedDataType().t_name == T_STRING || exp->containedDataType().t_name == T_UNSET) throw "Expression is either a string or null. This is unacceptable.";

        std::string destMemLoc = ""; //Destination Memory Location
        auto curr = MetaCoder::curr();

        if (lv->lvi == GLOBAL_REF) destMemLoc = std::to_string(lv->globalOffset) + "($gp)";
        else destMemLoc = std::to_string(lv->stackOffset) + "($sp)";
        auto gottenReg = exp->getRegister()->regName;
        curr->out << "\tsw " << gottenReg << "," << destMemLoc << "# This is the storage location of " << lv->idString << std::endl;
    }

    void declareConst(std::string cpsl_ref, std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();

        std::string id = cpsl_ref;

        if (find(curr->ids_toWrite.begin(), curr->ids_toWrite.end(), id) != curr->ids_toWrite.end()) {
            throw "data with existing id " + id + " is already being used.";
        }
        LValue newLV;
        newLV.idString = id;
        newLV.cpsl_refname = cpsl_ref;
        newLV.type = exp->containedDataType();

        if (exp->containedDataType().t_name == T_STRING) {
            newLV.lvi = DATA;
            curr->constExprs[cpsl_ref] = exp;

        }
        else {
            newLV.lvi = GLOBAL_REF;
            newLV.globalOffset = curr->topOfGlobal(4);
            curr->out << "\t#id: "<< id << " loaded into consts" << std::endl;
            curr->constExprs[cpsl_ref] = exp;
        }
        curr->LVs[cpsl_ref] = std::make_shared<LValue>(newLV);
    }

    void ReadValue(std::shared_ptr<LValue> lv) {
        if (lv->lvi == DATA || lv->lvi == CONST) throw "Can't read into constant memory locations.";
        auto curr = MetaCoder::curr();

        int MIPSReadType = 0;

        if (lv->type.t_name == T_INTEGER) MIPSReadType = 5;
        else if (lv->type.t_name == T_CHARACTER) MIPSReadType = 12;

        std::string destMemLoc = "";
        if (lv->lvi == GLOBAL_REF) destMemLoc = std::to_string(lv->globalOffset) + "($gp)";
        else if (lv->lvi == STACK_REF) destMemLoc = std::to_string(lv->stackOffset) + "($sp)";

        curr->out << "\tli $v0, " << MIPSReadType << "\n\tsyscall\n\tsw $v0," << destMemLoc << std::endl;
    }

    void WriteExpr(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        switch(exp->containedDataType().memBlkSize) {
            case 4: {
                if(exp->containedDataType().t_name == T_INTEGER || exp->containedDataType().t_name == T_BOOLEAN) {

                    curr->out << "\tli $v0, 1\n\tmove $a0, " << exp->getRegister()->regName << "\n\tsyscall\n";
                }
                else {
                    curr->out << "\tli $v0, 11\n\tmove $a0, " << exp->getRegister()->regName << "\n\tsyscall\n";
                }
                break;
            }

            case -1: {
                //This means it's a string. Ain't no way it'll be nothin' else
                //TODO: If string is not found within pre-existing consts...
                std::string searchFor = ExpToConstData(exp);
                std::cout << "TESTING " << exp->getStrVal() << std::endl;
                declareConst(searchFor,std::make_shared<Expression>(exp->getStrVal(), StringType()));

                std::shared_ptr<LValue> lvTemp = LVFromID(searchFor);
                auto expRet = ExprFromLV(lvTemp);

                curr->out << "\tli $v0, 4\n\tla $a0, " << expRet->getRegister()->regName << "\n\tsyscall\n";
                break;
            }
            default: {
                throw "Something horrible has happened. Your expression holds a non-standard data type, and is thus unprintable.";
            }
        }
    }
    Type SearchForSimple(std::string tString) {
        if (tString == "integer") return IntType();
        if (tString == "boolean") return BooleanType();
        if (tString == "character") return CharType();
        if (tString == "string") return StringType();

        throw("non-simple type lookup failed");
    }
/*****STRING HELPERS*****/
    std::string ExpToConstData(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        if (exp->containedDataType().t_name != T_STRING) {
//            throw "attempted to label non-string data type.";
            std::string dataLabel = "dataLabel" + std::to_string(curr->nextDataCtr());
            return dataLabel;
        }
        std::string stringLabel = "stringLabel" + std::to_string(curr->nextStringCtr());
        return stringLabel;
    }

    std::shared_ptr<MetaCoder> MetaCoder::_content = nullptr;

    std::shared_ptr<MetaCoder> MetaCoder::curr() {
        if(_content == nullptr) {
            RSWCOMP::MetaCoder::_content = std::make_shared<MetaCoder>();
            _content->out << ".globl main" << std::endl << std::endl << "main:" << std::endl;
        }
        return MetaCoder::_content;
    }

}

