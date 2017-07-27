//
// Created by Rulon Wood on 6/5/17.
//

#include "Register.h"

namespace RSWCOMP {

    std::shared_ptr<Register> Register::consumeRegister() {
        std::string consumedRegister;
        consumedRegister = rp.available.back();

        rp.available.pop_back();
        return std::make_shared<Register>(Register(consumedRegister));
    }
}