//
// Created by Rulon Wood on 6/12/17.
//

#ifndef SUMMERCOMPILER_METACODER_H
#define SUMMERCOMPILER_METACODER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include "its_complicated/components/LValue.h"

namespace RSWCOMP {

    void MainBlock();
    void ConstBlock();
    void WriteId(std::string id);
    void WriteVars(Type t);
    void Stop();
    Type LookupType(std::string tName);
    void WriteConsts();
    std::shared_ptr<LValue> LVFromID(std::string id);


    class MetaCoder {
        static std::shared_ptr<MetaCoder> _content;
        static std::string _outputFileName;
        int globalOffset =  0;
        int stackOffset = 0;




    public:
        static std::shared_ptr<MetaCoder> curr();
        std::unordered_map<std::string, std::shared_ptr<LValue>> LVs;
        std::ofstream out;

    };

    static std::shared_ptr<MetaCoder> cntxt;

}



#endif //SUMMERCOMPILER_METACODER_H
