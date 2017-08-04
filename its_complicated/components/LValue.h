//
// Created by Rulon Wood on 6/12/17.
//

#ifndef SUMMERCOMPILER_LVALUE_H
#define SUMMERCOMPILER_LVALUE_H

#include <string>
#include <memory>
#include "Type.h"

namespace RSWCOMP {
    //LValues point to specific memory locations; not intrinsic values.
    //e.g., addresses or stack positions rather than numbers or characters.
    enum LVInformation {
        GLOBAL_REF,
        STACK_REF,
        DATA,
        CONST
    };

    struct LValue {
        LVInformation lvi;
        Type type;
        int globalOffset;
        int stackOffset;
        int constVal;
        std::string name;
    };

    std::shared_ptr<LValue> loadFromId(std::string id);

}



#endif //SUMMERCOMPILER_LVALUE_H
