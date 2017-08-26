//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include <algorithm>
#include "LValue.h"
#include "its_complicated/MetaCoder.h"

namespace RSWCOMP {
    std::shared_ptr<LValue> loadId(std::string cpsl_id) {
        auto curr = MetaCoder::curr();
        auto found = curr->LVs.find(cpsl_id);

        if (found == curr->LVs.end()) {
            throw("No variable with provided cpsl_id exists.");
        }
        else {
            return found->second;
        }
    }

}