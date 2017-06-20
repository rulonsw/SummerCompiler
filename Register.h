//
// Created by Rulon Wood on 6/5/17.
//

#ifndef SUMMERCOMPILER_REGISTER_H
#define SUMMERCOMPILER_REGISTER_H

#include <string>
#include <vector>

namespace RSWCOMP {
    enum regDataType {ID, STRING, CHAR, INT};

    class Pool {
        std::vector<std::string> available = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                              "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
    };
    class Register {
    public:
        Pool rp;
        void consumeRegister();
        void releaseRegister();

    };

}


#endif //SUMMERCOMPILER_REGISTER_H
