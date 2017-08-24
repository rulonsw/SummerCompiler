//
// Created by Rulon Wood on 6/12/17.
//

#ifndef SUMMERCOMPILER_METACODER_H
#define SUMMERCOMPILER_METACODER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <its_complicated/components/Expression.h>
#include <vector>
#include <sstream>
#include "its_complicated/components/LValue.h"
#include "its_complicated/CtrlContext.h"

namespace RSWCOMP {

    void MainBlock();
    void ConstBlock();
    void WriteABlock();



    const std::shared_ptr<Expression> CharExpr(char c);
    const std::shared_ptr<Expression> IntExpr(int i);
    const std::shared_ptr<Expression> StringExpr(std::string s);

    void MakeVar(Type t);
    void MakeId(std::string s);

    const std::shared_ptr<Expression> ChrExpr(std::shared_ptr<Expression> expr);
    const std::shared_ptr<Expression> OrdExpr(std::shared_ptr<Expression> e);

    const std::shared_ptr<Expression> PredExpr(std::shared_ptr<Expression> e);
    const std::shared_ptr<Expression> SuccExpr(std::shared_ptr<Expression> e);

    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2);
    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2);
    const std::shared_ptr<Expression> NotExpr(std::shared_ptr<Expression> e);

    void declareConst(std::string id, std::shared_ptr<Expression> exp);
    std::string ExpToConstData(std::shared_ptr<Expression> exp);

    void ReadValue(std::shared_ptr<LValue> lv);
    void WriteExpr(std::shared_ptr<Expression> exp);
    Type SearchForSimple(std::string tString);
    Type LookupType(std::string tName); //To be implemented...

    void Assign(std::shared_ptr<LValue> lv, std::shared_ptr<Expression> exp);
/*****CONTROL FLOW*****/
    void Stop();
/*****IF/ELSE*****/
    void ProcIfStmt(std::shared_ptr<Expression> exp);
    void FinishIfStmt();
    void ProcElseStmt();
    void PrepElseIfStmt();
    void ProcElseIfStmt(std::shared_ptr<Expression> exp);
/*****LOOPS*****/
    void ProcWhileStmt(std::shared_ptr<Expression> exp);
    void PrepWhileStmt();
    void FinishWhileStmt();

    void ProcRepeatStmt(std::shared_ptr<Expression> exp);
    void PrepRepeatStmt();

    void ProcForStmt(std::string varId, std::shared_ptr<Expression> exp);
    void PrepForStmt(bool direction);
    void ProcToHead(std::shared_ptr<Expression> exp);
    void ProcDownToHead(std::shared_ptr<Expression> exp);

    std::shared_ptr<Expression> ExprFromLV(std::shared_ptr<LValue> lv);
    std::shared_ptr<LValue> LVFromID(std::string id);

    /*****MATH SECTION*****/

    const std::shared_ptr<Expression> AddExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> SubExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);

    const std::shared_ptr<Expression> MultExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> DivExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> ModExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> UnMinusExpr(std::shared_ptr<Expression>e);

    const std::shared_ptr<Expression> EqExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> NeqExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);

    const std::shared_ptr<Expression> GteExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> GtExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);

    const std::shared_ptr<Expression> LteExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);
    const std::shared_ptr<Expression> LtExpr(std::shared_ptr<Expression>e1, std::shared_ptr<Expression>e2);


    class MetaCoder {
        static std::shared_ptr<MetaCoder> _content;
        int globalOffset =  0;
        int stackOffset = 0;
        int stringCounter = 0;
        int dataCounter = 0;

        int numConditionalBlocks = -1;
        int depth = 0;

    public:

        CtrlContext whileContext;
        CtrlContext repeatContext;
        CtrlContext forContext;

        std::vector<std::string> tosToSort;
        std::vector<std::string> toUps;
        std::vector<std::string> toDowns;

        MetaCoder();
        ~MetaCoder();
        std::ofstream out;

        std::stringstream constBlockToWrite;
        std::stringstream mainBlockToWrite;

        std::stringstream intermediateBlock;
        void dumpToMain();

        int getDepth() {return depth;}
        void shallow() {depth -= 1;}
        void deep() {
            depth +=1;
            if (depth >= whileContext.getMaxDepth()){
                whileContext.deepen();
                repeatContext.deepen();
                forContext.deepen();
            }

        }
        int getConditionalBlkNum() {return numConditionalBlocks;}
        int incrConditionalBlkNum() {
            elseBlockLabels[numConditionalBlocks+1] = 0;
            return ++numConditionalBlocks;
        }
        int exitConditionalLayer();

//        int getNumWhileBlks() {return numWhileBlocks;}
//        int incrWhileBlks() {return ++numWhileBlocks;}

        int topOfGlobal() {
            int i = globalOffset;
            globalOffset += 4;
            return i;
        }
        int topOfGlobal(int i){
            //Number of discrete variables involved in user-defined class held in arglist * 4
            int j = globalOffset;
            globalOffset += i;
            return j;
        }

        static std::string _outputFileName;
        static std::shared_ptr<MetaCoder> curr();

        std::vector<std::string> ids_toWrite;

        std::map<int, int> elseBlockLabels;
        int nextElseBlockLabel() {
            elseBlockLabels[numConditionalBlocks] += 1;
            return elseBlockLabels[numConditionalBlocks];
        }

        std::unordered_map<std::string, std::shared_ptr<LValue>> LVs;
        std::unordered_map<std::string, std::shared_ptr<Expression>> constExprs;

        int nextStringCtr() {
            int ret = stringCounter;
            stringCounter++;

            return ret;
        }
        int nextDataCtr() {
            int ret = dataCounter;
            dataCounter++;

            return ret;
        }

    };
}



#endif //SUMMERCOMPILER_METACODER_H
