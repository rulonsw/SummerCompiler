//
// Created by Rulon Wood on 6/12/17.
//

#include <iostream>
#include "LValue.h"
#include "its_complicated/MetaCoder.h"

namespace RSWCOMP {
    std::shared_ptr<LValue> LoadId(std::string id) {
        auto curr = MetaCoder::curr();
        auto found = curr->LVs.find(id);

        if (found == curr->LVs.end()) {
            LValue l;
            //TODO: Add LValue of this type to the LValue unordered_map, LVs
        }
        else std::cout << "variable id " << id << " has already been defined." << std::endl;
        exit(1);
    }

}