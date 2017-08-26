//
// Created by Rulon Wood on 6/5/17.
//

#ifndef SUMMERCOMPILER_REGISTER_H
#define SUMMERCOMPILER_REGISTER_H

#include <string>
#include <vector>

namespace RSWCOMP {
    enum regDataType {ID, STRING, CHAR, INT};


    class Register {
    private:
        static std::vector<std::string> available;

    public:
        std::string regName;
        Register(std::string n)
                :regName(n) {};

        ~Register() {
            available.push_back(this->regName);
        };

        static std::shared_ptr<Register> consumeRegister();

        static std::vector<std::string> usedRegisters() {
            std::vector<std::string> usedUp = {"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                                               "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"};
            for (auto i : available) {
                auto found = find(usedUp.begin(), usedUp.end(), i);
                if (found != usedUp.end()) usedUp.erase(found);
            }
            return usedUp;
        }
    };

}


#endif //SUMMERCOMPILER_REGISTER_H
