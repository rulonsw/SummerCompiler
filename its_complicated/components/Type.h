//
// Created by Rulon Wood on 6/13/17.
//

#include <cstdint>
#include <string>

#ifndef TYPE_H
#define TYPE_H

namespace RSWCOMP {
    enum TYPE_NAME {
        T_VOID,
        T_INTEGER,
        T_BOOLEAN,
        T_CHARACTER,
        T_STRING
    };
    class Type {
    public:
        ~Type() = default;
        TYPE_NAME t_name = T_VOID;
        int memBlkSize = INT32_MIN;
    };

    class IntType: public Type {
    public:
        IntType() {
            memBlkSize = 4;
            t_name = T_INTEGER;
        }
    };

    class BooleanType: public Type {
    public:
        BooleanType() {
            memBlkSize = 4;
            t_name = T_BOOLEAN;
        }
    };

    class CharType: public Type {
    public:
        CharType() {
            memBlkSize = 4;
            t_name = T_CHARACTER;
        }
    };

    class StringType: public Type {
    public:
        StringType() {
            memBlkSize = -1;
            t_name = T_STRING;
        }
    };
}
#endif //SUMMERCOMPILER_TYPE_H