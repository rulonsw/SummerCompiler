//
// Created by Rulon Wood on 6/5/17.
//

#include "Register.h"

namespace RSWCOMP {

    std::vector<std::string> Register::available = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                                           "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};

    std::shared_ptr<Register> Register::consumeRegister() {

        if(available.empty() == true) {
            throw "Register pool is empty. Abort!";
        }
        std::string consumedRegister;
        consumedRegister = available.back();

        available.pop_back();
        return std::make_shared<Register>(consumedRegister);
    }

    static std::vector<std::string> Register::usedRegisters() {
        std::vector<std::string> usedUp =  {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                                  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
        for (auto i : available) {
            auto found = find(usedUp.begin(), usedUp.end(), i);
            if(found != usedUp.end()) usedUp.erase(found);
        }
        return usedUp;
    }
}