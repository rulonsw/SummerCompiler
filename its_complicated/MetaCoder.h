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

namespace RSWCOMP {

    void MainBlock();
    void ConstBlock();
    void dumpToMain();


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
    void ProcIfStmt(std::shared_ptr<Expression> exp);
    void FinishIfStmt();
    void ProcElseStmt();
    void ProcElseIfStmt(std::shared_ptr<Expression> exp);
    void ProcElseIfStmt();
    void FinishElseIfStmt();

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
        int numConditionalBlocks = 0;
        int controlDepth = 0;


    public:

        MetaCoder();
        ~MetaCoder();
        std::ofstream out;

        std::stringstream constBlockToWrite;
        std::stringstream mainBlockToWrite;
        std::stringstream intermediateBlock;

        //TODO: Consider revising this. Depth doesn't need to be taken into account, as \
                Each nested if statement will simply jump to its appropriate subroutine.
        //True: Increase nest count
        //False: Go up a level in the nest
        void changeBlkScope(bool nestFurther) {
            controlDepth = nestFurther? controlDepth +1 :
                                   controlDepth == 0? 0 :controlDepth -1;
        }
        int getConditionalBlkNum() {return numConditionalBlocks;}

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
