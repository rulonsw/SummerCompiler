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
                :regName(n){};

        ~Register(){
            available.push_back(this->regName);
        };
        static std::shared_ptr<Register> consumeRegister();

    };

}


#endif //SUMMERCOMPILER_REGISTER_H
