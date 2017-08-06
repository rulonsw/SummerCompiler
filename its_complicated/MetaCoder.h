//
// Created by Rulon Wood on 6/12/17.
//

#ifndef SUMMERCOMPILER_METACODER_H
#define SUMMERCOMPILER_METACODER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <its_complicated/components/Expression.h>
#include <vector>
#include "its_complicated/components/LValue.h"

namespace RSWCOMP {

    void MainBlock();
    void ConstBlock();
    void WriteId(std::string id);

    const std::shared_ptr<Expression> CharExpr(char c);
    const std::shared_ptr<Expression> ChrExpr(std::shared_ptr<Expression> expr);

    const std::shared_ptr<Expression> AndExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2);
    const std::shared_ptr<Expression> OrExpr(std::shared_ptr<Expression> e1, std::shared_ptr<Expression> e2);

    void WriteVars(Type t);

    void declareConst(std::string id, std::shared_ptr<Expression> exp);
    std::string StringToAsciizData(std::shared_ptr<Expression> exp);

    void ReadValue(std::shared_ptr<LValue> lv);
    void WriteExpr(std::shared_ptr<Expression> exp);

    void Assign(std::shared_ptr<LValue> lv, std::shared_ptr<Expression> exp);

    void Stop();

    Type LookupType(std::string tName);
    void WriteConsts();

    std::shared_ptr<Expression> ExprFromLV(std::shared_ptr<LValue> lv);
    std::shared_ptr<LValue> LVFromID(std::string id);


    class MetaCoder {
        static std::shared_ptr<MetaCoder> _content;
        static std::string _outputFileName;
        int globalOffset =  0;
        int stackOffset = 0;
        int stringCounter = 0;


    public:
        int topOfGlobal() {
            int i = globalOffset;
            globalOffset += 4;
            return i;
        }
        int topOfGlobal(int i){
            //Number of discrete variables involved in user-defined class held in arglist
            int j = globalOffset;
            globalOffset += (4 * i);
            return j;
        }
        static std::shared_ptr<MetaCoder> curr();
        static std::vector<std::string> existingIds;
        std::unordered_map<std::string, std::shared_ptr<LValue>> LVs;
        std::unordered_map<std::string, std::shared_ptr<Expression>> constExprs;
        std::ofstream out;
        int nextStringCtr() {
            int ret = stringCounter;
            stringCounter++;

            return ret;
        }

    };

    static std::shared_ptr<MetaCoder> cntxt;

}



#endif //SUMMERCOMPILER_METACODER_H
