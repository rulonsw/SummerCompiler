//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include <boost/algorithm/string.hpp>
#include "MetaCoder.h"
namespace RSWCOMP {
/****Sections****/
    void MainBlock() {
        auto curr = MetaCoder::curr();
        ConstBlock();
        curr->constBlockToWrite << "\n\n\n\t.text\n";
        curr->out << curr->constBlockToWrite.str() << curr->mainBlockToWrite.str() << std::endl;
        Stop();
    }

    void ConstBlock() {
        static bool firstPassTaken = false;

        auto curr = MetaCoder::curr();

        if (!firstPassTaken) {
            curr->constBlockToWrite << "\t.data\n";
            firstPassTaken = true;
        }

        for (auto foo : curr->constExprs) {

            if (foo.second->containedDataType().t_name == T_STRING) {
                curr->constBlockToWrite << foo.first << ": .asciiz " << "\"" << foo.second->getStrVal() << "\"" << std::endl;
            }
            else {
                curr->constBlockToWrite << foo.first << ": .word " << foo.second->getNumericValue() << std::endl;
            }
        }
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

        auto expr = std::make_shared<Expression>("", StringType());
        switch (tempLV->lvi) {
            case GLOBAL_REF:
                expr = std::make_shared<Expression>(Register::consumeRegister(), tempLV->type);
                expr->exprId = lv->idString;
                curr->mainBlockToWrite << "\tlw " << expr->getRegister()->regName <<", " << tempLV->globalOffset << "($gp)" << std::endl;
                return (expr);
            case STACK_REF:
                expr = std::make_shared<Expression>(Register::consumeRegister(), tempLV->type);
                expr->exprId = lv->idString;
                curr->mainBlockToWrite << "\tlw " << expr->getRegister()->regName <<", " << tempLV->stackOffset << "($sp)" << std::endl;
                return (expr);
            case DATA:
                expr = std::make_shared<Expression>(Register::consumeRegister(), tempLV->type);
                expr->exprId = lv->idString;
                return (expr);
            case CONST:
                expr = std::make_shared<Expression>(tempLV->constVal, tempLV->type);
                expr->exprId = lv->idString;
                return expr;
            default:
                throw "Unknown LV type present in tempLV." ;
        }
    }

/*****BASIC CONTROL FLOW*****/
    void Stop() {
        auto curr = MetaCoder::curr();
        curr->out << "li $v0, 10" << std::endl;
        curr->out << "syscall" << std::endl;
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
            lv.cpsl_refname = foo;
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

        if(e->containedDataType().t_name == T_CHARACTER || e->containedDataType().t_name == T_INTEGER) {
            curr->mainBlockToWrite << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", -1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->mainBlockToWrite << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(false);
        return e;

    }
    const std::shared_ptr<Expression> SuccExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();

        if(e->containedDataType().t_name == T_CHARACTER || e->containedDataType().t_name == T_INTEGER) {
            curr->mainBlockToWrite << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", 1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->mainBlockToWrite << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(true);
        return e;
    }

/*****BOOLEAN OPERATIONS*****/
    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite << "\tand " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite << "\tor " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NotExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tnot " << result_register->regName << "," << e->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

/*****MATH OPERATIONS*****/
    const std::shared_ptr<Expression> AddExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite <<"\tadd " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> SubExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite <<"\tsub " << result_register->regName << ", " << e1->getRegister()->regName << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> MultExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite <<"\tmul " <<result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> DivExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        curr->mainBlockToWrite <<"\tdiv " <<result_register->regName << "," << e1->getRegister() << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> ModExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tdiv " << e1->getRegister()->regName << "," << e2->getRegister()-> regName << "\n\tmfhi " << result_register->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> UnMinusExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tneg " << result_register->regName << "," << e->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

    const std::shared_ptr<Expression> EqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tseq " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NeqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tsne " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> GteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tsge " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> GtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tsgt " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tsle " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->mainBlockToWrite << "\tslt " << result_register->regName << "," << e1->getRegister()->regName << "," << e2->getRegister()->regName << std::endl;

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
        curr->mainBlockToWrite << "\tsw " << gottenReg << "," << destMemLoc << "# This is the storage location of " << lv->idString << std::endl;
    }

    void declareConst(std::string cpsl_ref, std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();

        std::string id = cpsl_ref;

        if (find(curr->ids_toWrite.begin(), curr->ids_toWrite.end(), id) != curr->ids_toWrite.end()) {
            throw "data with existing id " + id + " is already being used.";
        }
        LValue newLV;
        newLV.idString = cpsl_ref;
        newLV.cpsl_refname = cpsl_ref;
        newLV.type = exp->containedDataType();

        if (exp->containedDataType().t_name == T_STRING) {
            newLV.lvi = DATA;
        }
        else {
            newLV.lvi = CONST;
            curr->mainBlockToWrite << "\t#id: "<< id << " loaded into consts" << std::endl;
        }
        curr->constExprs[cpsl_ref] = exp;
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

        curr->mainBlockToWrite << "\tli $v0, " << MIPSReadType << "\n\tsyscall\n\tsw $v0," << destMemLoc << std::endl;
    }

    void WriteExpr(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        switch(exp->containedDataType().memBlkSize) {
            case 4: {
                auto gottenRegister = exp->getRegister()->regName;
                if(exp->containedDataType().t_name == T_INTEGER || exp->containedDataType().t_name == T_BOOLEAN) {
                    curr->mainBlockToWrite << "\tli $v0, 1\n\tmove $a0, " << gottenRegister << "\n\tsyscall\n";
                }
                else {
                    curr->mainBlockToWrite << "\tli $v0, 11\n\tmove $a0, " << gottenRegister << "\n\tsyscall\n";
                }
                break;
            }

            case -1: {
                //This means it's a string. Ain't no way it'll be nothin' else

                //If string doesn't already have an ID:
                std::string searchFor = exp->exprId;
                if (searchFor == "UNSET_ID") {
                    exp->exprId = ExpToConstData(exp);
                    declareConst(exp->exprId,std::make_shared<Expression>(exp->getStrVal(), StringType()));
                }
                std::shared_ptr<LValue> lvTemp = LVFromID(exp->exprId);
                auto expRet = ExprFromLV(lvTemp);

                curr->mainBlockToWrite << "\tli $v0, 4\n\tla $a0, " << expRet->exprId << "\n\tsyscall\n";
                break;
            }
            default: {
                throw "Something horrible has happened. Your expression holds a non-standard data type, and is thus unprintable.";
            }
        }
    }
    Type SearchForSimple(std::string tString) {
        boost::algorithm::to_lower(tString);
        if (tString == "integer") return IntType();
        if (tString == "boolean") return BooleanType();
        if (tString == "char") return CharType();
        if (tString == "string") return StringType();

        throw("non-simple type lookup failed");
    }
/*****STRING HELPERS*****/
    std::string ExpToConstData(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        if (exp->containedDataType().t_name != T_STRING) {
            /*This may happen in the event that a Read() method is called. */
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
            _content->mainBlockToWrite << ".globl main\n\nmain:\n";

            declareConst("true", std::make_shared<Expression>(1, BooleanType()));
            declareConst("TRUE", std::make_shared<Expression>(1, BooleanType()));
            declareConst("false", std::make_shared<Expression>(0, BooleanType()));
            declareConst("FALSE", std::make_shared<Expression>(0, BooleanType()));
         }
        return MetaCoder::_content;
    }

}

