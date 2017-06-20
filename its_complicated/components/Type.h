//
// Created by Rulon Wood on 6/13/17.
//

#include <cstdint>
#include <string>

#ifndef SUMMERCOMPILER_TYPE_H
#define SUMMERCOMPILER_TYPE_H

#endif //SUMMERCOMPILER_TYPE_H
namespace RSWCOMP {
    class Type {
    public:
        ~Type() = default;
        std::string typeName = "";
        int memBlkSize = INT32_MIN;
    };

    class IntType: public Type {
        IntType() {
            memBlkSize = 4;
            typeName = "Integer";
        }
    };

    class BooleanType: public Type {
        BooleanType() {
            memBlkSize = 4;
            typeName = "Boolean";
        }
    };

    class CharType: public Type {
        CharType() {
            memBlkSize = 4;
            typeName = "Character";
        }
    };

    class StringType: public Type {
        StringType() {
            memBlkSize = -1;
            typeName = "String";
        }
    };
}