//
// Created by Rulon Wood on 6/12/17.
//

#include "MetaCoder.h"

namespace RSWCOMP {

    std::shared_ptr<MetaCoder> MetaCoder::_content = nullptr;
    std::string MetaCoder::_outputFileName = "output.asm";

    std::shared_ptr<MetaCoder> MetaCoder::curr() {
        if(_content == nullptr) {
            std::make_shared<MetaCoder>();
            _content->out << ".globl main" << std::endl << std::endl << "main:" << std::endl;

        }
        return MetaCoder::_content;
    }

}

