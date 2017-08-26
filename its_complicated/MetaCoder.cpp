//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include <boost/algorithm/string.hpp>
#include "MetaCoder.h"
namespace RSWCOMP {
/****SECTIONS****/
    void MainBlock() {
        auto curr = MetaCoder::curr();
        ConstBlock();
        curr->constBlockToWrite << "\n\n\n\t.text\n";
        curr->out << curr->constBlockToWrite.str() << curr->mainBlockToWrite.str() << std::endl;
        Stop();
    }
    void WriteABlock() {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
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
/*****METACODER THINGS*****/

    std::shared_ptr<MetaCoder> MetaCoder::_content = nullptr;

    void MetaCoder::dumpToMain() {
        auto curr = MetaCoder::curr();
        curr->mainBlockToWrite << curr->intermediateBlock.str();

        curr->intermediateBlock.str(std::string());
    }

    int MetaCoder::exitConditionalLayer() {
        auto ret = numConditionalBlocks;
        numConditionalBlocks--;
        elseBlockLabels.erase(ret);
        return ret;
    }

    std::shared_ptr<MetaCoder> MetaCoder::curr() {
        if(_content == nullptr) {
            RSWCOMP::MetaCoder::_content = std::make_shared<MetaCoder>();
            _content->intermediateBlock << ".globl main\n\nmain:\n";

            declareConst("true", std::make_shared<Expression>(1, BooleanType()), nullptr);
            declareConst("TRUE", std::make_shared<Expression>(1, BooleanType()), nullptr);
            declareConst("false", std::make_shared<Expression>(0, BooleanType()), nullptr);
            declareConst("FALSE", std::make_shared<Expression>(0, BooleanType()), nullptr);
            _content->dumpToMain();
        }
        return MetaCoder::_content;
    }

    std::string MetaCoder::_outputFileName = "out.asm";
    MetaCoder::MetaCoder() {
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
                curr->intermediateBlock << "\tlw " << expr->getRegister()->regName <<", " << tempLV->globalOffset << "($gp)" << std::endl;
                return (expr);
            case STACK_REF:
                expr = std::make_shared<Expression>(Register::consumeRegister(), tempLV->type);
                expr->exprId = lv->idString;
                curr->intermediateBlock << "\tlw " << expr->getRegister()->regName <<", " << tempLV->stackOffset << "($sp)" << std::endl;
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

/***** CONTROL FLOW*****/
    void Stop() {
        auto curr = MetaCoder::curr();
        curr->out << "li $v0, 10" << std::endl;
        curr->out << "syscall" << std::endl;
    }
    /*****IF/ELSE*****/
    void ProcIfStmt(std::shared_ptr<Expression> expr) {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
        curr->mainBlockToWrite << "\tbeq " << expr->getRegister()->regName << ", $0, "
                                    << "elseBlock_" << curr->incrConditionalBlkNum() << "_num" << curr->nextElseBlockLabel() << std::endl;
    }
    void FinishIfStmt() {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
        curr->mainBlockToWrite << "elseBlock_" << curr->exitConditionalLayer() << "_end:" << std::endl;
    }
    void ProcElseIfStmt(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
        curr->mainBlockToWrite << "\tbeq "<< exp->getRegister()->regName <<", $0, elseBlock_" << curr->getConditionalBlkNum() << "_num" << curr->nextElseBlockLabel()  << std::endl;
    }
    void PrepElseIfStmt() {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
        curr->mainBlockToWrite << "j elseBlock_" << curr->getConditionalBlkNum() << "_end" << std::endl;
        curr->mainBlockToWrite << "elseBlock_" << curr->getConditionalBlkNum() << "_num" << curr->elseBlockLabels[curr->getConditionalBlkNum()] << ":" << std::endl;
    }
    void ProcElseStmt() {
        auto curr = MetaCoder::curr();
        curr->dumpToMain();
        curr->mainBlockToWrite << "j elseBlock_" << curr->getConditionalBlkNum() << "_end" << std::endl;
        curr->mainBlockToWrite << "elseBlock_" << curr->getConditionalBlkNum() << "_num" << curr->elseBlockLabels[curr->getConditionalBlkNum()] << ":" << std::endl;
    }

    /*****LOOPS*****/
    void ProcWhileStmt(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        auto depth = curr->getDepth();
        curr->dumpToMain();
        curr->mainBlockToWrite << "\tbeq " << exp->getRegister()->regName << ", $0, whileNum" << curr->whileContext.getIdAtDepth(depth) << "_depth" << curr->getDepth() << "_end" << std::endl;
    }
    void PrepWhileStmt() {
        auto curr= MetaCoder::curr();
        curr->deep();
        auto depth = curr->getDepth();
        curr->dumpToMain();
        curr->mainBlockToWrite << "whileNum" << curr->whileContext.pushCtrlAtDepth(depth) << "_depth" << depth <<":" << std::endl;
    }
    void FinishWhileStmt() {
        auto curr = MetaCoder::curr();
        auto depth = curr->getDepth();

        curr->dumpToMain();
        curr->mainBlockToWrite << "j whileNum" << curr->whileContext.getIdAtDepth(depth) << "_depth" << depth <<std::endl;
        curr->mainBlockToWrite << "whileNum" << curr->whileContext.getIdAtDepth(depth) <<"_depth" << depth << "_end:" << std::endl;
        curr->shallow();
    }

    void ProcRepeatStmt(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        auto depth = curr->getDepth();
        curr->dumpToMain();
        curr->mainBlockToWrite << "\tbne " << exp->getRegister()->regName << ", $0, repeatNum" << curr->repeatContext.getIdAtDepth(depth) << "_depth" << depth <<"_end"<< std::endl;
        curr->mainBlockToWrite << "j repeatNum" << curr->repeatContext.getIdAtDepth(depth) << "_depth" << depth << std::endl;
        curr->mainBlockToWrite << "repeatNum" << curr->repeatContext.getIdAtDepth(depth) << "_depth" << depth << "_end:" << std::endl;

        curr->shallow();
    }
    void PrepRepeatStmt() {
        auto curr = MetaCoder::curr();
        curr->deep();
        auto depth = curr->getDepth();
        curr->dumpToMain();
        curr->mainBlockToWrite << "repeatNum" << curr->repeatContext.pushCtrlAtDepth(depth) << "_depth" << depth << ":" << std::endl;
    }

    void ProcForStmt(std::string varId, std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();

        auto lv = LVFromID(varId);
        Assign(lv, exp);
        curr->tosToSort.push_back(varId);

        curr->dumpToMain();

    }
    void PrepForStmt(bool direction) {
        //True: To
        //False: DownTo
        auto curr = MetaCoder::curr();
        curr->dumpToMain();

        auto ForIdToPrep = direction? curr->toUps.back() : curr->toDowns.back();
        auto lvTemp = LVFromID(ForIdToPrep);

        if (direction) {
            curr->toUps.pop_back();
            Assign(lvTemp, SuccExpr(ExprFromLV(lvTemp)));
        }
        else {
            curr->toDowns.pop_back();
            Assign(lvTemp, PredExpr(ExprFromLV(lvTemp)));
        }
        curr->dumpToMain();

        curr->mainBlockToWrite << "j forNum" << curr->forContext.getIdAtDepth(curr->getDepth()) <<"_depth" << curr->getDepth() << std::endl;
        curr->mainBlockToWrite << "forNum" << curr->forContext.getIdAtDepth(curr->getDepth()) << "_depth" << curr->getDepth() << "_end:" << std::endl;
        curr->shallow();

    }
    void ProcToHead(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        curr->deep();
        auto depth = curr->getDepth();
        curr->mainBlockToWrite << "forNum" << curr->forContext.pushCtrlAtDepth(depth) << "_depth" << depth << ":" << std::endl;
        auto addToUp = curr->tosToSort.back(); //CLion throws an error here; alas, it is for naught.
        //Seems like the issue related to this ticket in particular: https://youtrack.jetbrains.com/issue/CPP-8163
        curr->tosToSort.pop_back();
        curr->toUps.push_back(addToUp);

        auto compReg1 = std::const_pointer_cast<Expression>(ExprFromLV(LVFromID(addToUp)));
        auto compReg1Name = compReg1->getRegister()->regName;
        auto compReg2 = exp->getRegister()->regName;
        curr->dumpToMain();

        curr->mainBlockToWrite << "beq " << compReg1Name << ", " << compReg2 << ", forNum" << curr->forContext.getIdAtDepth(depth)<< "_depth" << depth <<"_end" << std::endl;


    }
    void ProcDownToHead(std::shared_ptr<Expression> exp) {

        auto curr = MetaCoder::curr();
        curr->deep();
        auto depth = curr->getDepth();
        curr->mainBlockToWrite << "forNum" << curr->forContext.pushCtrlAtDepth(depth) << "_depth" << depth << ":" << std::endl;
        auto addToDown = curr->tosToSort.back(); //CLion throws an error here; alas, it is for naught.
        //Seems like the issue related to this ticket in particular: https://youtrack.jetbrains.com/issue/CPP-8163

        curr->tosToSort.pop_back();
        curr->toDowns.push_back(addToDown);
        auto compReg1 = std::const_pointer_cast<Expression>(ExprFromLV(LVFromID(addToDown)));
        auto compReg1Name = compReg1->getRegister()->regName;
        auto compReg2 = exp->getRegister()->regName;
        curr->dumpToMain();

        curr->mainBlockToWrite << "beq " << compReg1Name << ", " << compReg2 << ", forNum" << curr->forContext.getIdAtDepth(depth)<< "_depth" << depth <<"_end" << std::endl;

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

    //done: reform MakeId with scope sensitivity
    void MakeId(std::string s, std::shared_ptr<std::string> calledFrom) {
        auto curr = MetaCoder::curr();
        if (calledFrom != nullptr) s = s.append(("_" + *calledFrom));
        curr->ids_toWrite.push_back(s);
    }
    //done: reform MakeVar with scope sensitivity
    void MakeVar(Type t, std::shared_ptr<std::string> calledFrom) {
        /*This method is essentially only called when there's 1+ things in the `ids_toWrite` vector.*/
        auto curr = MetaCoder::curr();

        if(curr->ids_toWrite.size() == 0) throw("Variable creation attempt with empty list of var names.");

        while(curr->ids_toWrite.size() != 0) {
            auto foo = curr->ids_toWrite[0];

            if(calledFrom != nullptr) foo = foo.append(("_" + *calledFrom));

            auto isFound = curr->LVs.find(foo);
            if (isFound != curr->LVs.end()) {
                throw("Duplicate LValue idString found during compilation. Please retry using distinct var names for each variable.");
            }
            LValue lv;

            lv.lvi = calledFrom == nullptr? GLOBAL_REF : STACK_REF;
            lv.idString = foo;
            lv.cpsl_refname = foo;
            if (calledFrom == nullptr) lv.globalOffset = curr->topOfGlobal(t.memBlkSize);
            else lv.stackOffset = curr->topOfStack(t.memBlkSize);
            lv.type = t;
            if (calledFrom == nullptr) lv.isLocal = true;
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
            curr->intermediateBlock << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", -1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->intermediateBlock << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(false);
        return e;

    }
    const std::shared_ptr<Expression> SuccExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();

        if(e->containedDataType().t_name == T_CHARACTER || e->containedDataType().t_name == T_INTEGER) {
            curr->intermediateBlock << "\taddi " << e->getRegister()->regName << "," << e->getRegister()->regName << ", 1" << std::endl;
        }
        if(e->containedDataType().t_name == T_BOOLEAN) {
            curr->intermediateBlock << "\tnot " << e->getRegister()->regName << "," << e->getRegister()->regName << std::endl;
        }
        e->step(true);
        return e;
    }

/*****BOOLEAN OPERATIONS*****/
    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock << "\tand " << result_register->regName << ", " << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock << "\tor " << result_register->regName << ", " << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NotExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();

        curr->intermediateBlock << "\tnot " << result_register->regName << "," << e->getRegister()->regName << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

/*****MATH OPERATIONS*****/
    const std::shared_ptr<Expression> AddExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock <<"\tadd " << result_register->regName << ", " << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> SubExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock <<"\tsub " << result_register->regName << ", " << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> MultExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock <<"\tmul " <<result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> DivExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;
        
        curr->intermediateBlock <<"\tdiv " <<result_register->regName << "," << e1->getRegister() << "," << e2->getRegister() << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> ModExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tdiv " << gottenReg1 << "," << e2->getRegister()-> regName << "\n\tmfhi " << result_register->regName << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> UnMinusExpr(std::shared_ptr<Expression> e) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg = e->getRegister()->regName;
        

        curr->intermediateBlock << "\tneg " << result_register->regName << "," << gottenReg << std::endl;

        return std::make_shared<Expression>(result_register, e->containedDataType());
    }

    const std::shared_ptr<Expression> EqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto e1GottenReg = e1->getRegister()->regName;
        auto e2GottenReg = e2->getRegister()->regName;

        curr->intermediateBlock << "\tseq " << result_register->regName << "," << e1GottenReg << "," << e2GottenReg << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> NeqExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tsne " << result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }

    const std::shared_ptr<Expression> GteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tsge " << result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> GtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tsgt " << result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LteExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tsle " << result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

        return std::make_shared<Expression>(result_register, e1->containedDataType());
    }
    const std::shared_ptr<Expression> LtExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2) {
        auto curr = MetaCoder::curr();
        auto result_register = Register::consumeRegister();
        auto gottenReg1 = e1->getRegister()->regName;
        auto gottenReg2 = e2->getRegister()->regName;

        curr->intermediateBlock << "\tslt " << result_register->regName << "," << gottenReg1 << "," << gottenReg2 << std::endl;

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
        curr->intermediateBlock << "\tsw " << gottenReg << "," << destMemLoc << "# This is the storage location of " << lv->idString << std::endl;
    }

//DONE: Refactor declareConst to include an optional argument of type std::string that indicates the function that consts are being declared for
    void declareConst(std::string cpsl_ref, std::shared_ptr<Expression> exp, std::shared_ptr<std::string> calledFrom) {
        auto curr = MetaCoder::curr();
        if (find(curr->ids_toWrite.begin(), curr->ids_toWrite.end(), cpsl_ref) != curr->ids_toWrite.end()) {
            throw "data with existing id " + cpsl_ref + " is already being used.";
        }
        LValue newLV;
        newLV.isLocal = !curr->getScope();
        newLV.idString = calledFrom == nullptr ? cpsl_ref : cpsl_ref + "_" + *calledFrom;
        newLV.cpsl_refname = calledFrom == nullptr ? cpsl_ref : cpsl_ref + "_" + *calledFrom;
        newLV.type = exp->containedDataType();

        if (exp->containedDataType().t_name == T_STRING) {
            newLV.lvi = DATA;
        }
        else {
            newLV.lvi = CONST;
            curr->intermediateBlock << "\t#id: "<< cpsl_ref << " loaded into " << (calledFrom == nullptr ?  "" : "local ") << "consts" << std::endl;
        }
        curr->constExprs[newLV.idString] = exp;
        curr->LVs[newLV.idString] = std::make_shared<LValue>(newLV);
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

        curr->intermediateBlock << "\tli $v0, " << MIPSReadType << "\n\tsyscall\n\tsw $v0," << destMemLoc << std::endl;
    }
    void WriteExpr(std::shared_ptr<Expression> exp) {
        auto curr = MetaCoder::curr();
        switch(exp->containedDataType().memBlkSize) {
            case 4: {
                auto gottenRegister = exp->getRegister()->regName;
                if(exp->containedDataType().t_name == T_INTEGER || exp->containedDataType().t_name == T_BOOLEAN) {
                    curr->intermediateBlock << "\tli $v0, 1\n\tmove $a0, " << gottenRegister << "\n\tsyscall\n";
                }
                else {
                    curr->intermediateBlock << "\tli $v0, 11\n\tmove $a0, " << gottenRegister << "\n\tsyscall\n";
                }
                break;
            }

            case -1: {
                //This means it's a string. Ain't no way it'll be nothin' else

                //If string doesn't already have an ID:
                std::string searchFor = exp->exprId;
                if (searchFor == "UNSET_ID") {
                    exp->exprId = ExpToConstData(exp);
                    declareConst(exp->exprId,std::make_shared<Expression>(exp->getStrVal(), StringType()), nullptr);
                }
                std::shared_ptr<LValue> lvTemp = LVFromID(exp->exprId);
                auto expRet = ExprFromLV(lvTemp);

                curr->intermediateBlock << "\tli $v0, 4\n\tla $a0, " << expRet->exprId << "\n\tsyscall\n";
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



}

