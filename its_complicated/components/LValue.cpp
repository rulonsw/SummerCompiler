//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include "LValue.h"
#include "its_complicated/MetaCoder.h"

namespace RSWCOMP {
    std::shared_ptr<LValue> loadId(std::string id) {
        auto curr = MetaCoder::curr();
        auto found = curr->LVs.find(id);

        if (found == curr->LVs.end()) {
            throw("No variable with provided id exists.");
        }
        else {
            return found->second;
        }
    }

}