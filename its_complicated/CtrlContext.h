//
// Created by Rulon Wood on 8/23/17.
//

#ifndef SUMMERCOMPILER_CTRLCONTEXT_H
#define SUMMERCOMPILER_CTRLCONTEXT_H
#include <vector>
#include <map>

namespace RSWCOMP {
    class CtrlContext {
        int maxDepth = -1;
        std::vector<int> maxIdAtDepth;
        std::map<int, std::vector<int>> depthIdStack;
    public:
        CtrlContext();
        int getMaxDepth();
        int getIdAtDepth(int i);
        int popIdAtDepth(int i);
        int pushCtrlAtDepth(int i);
        void deepen();

    };
}



#endif //SUMMERCOMPILER_CTRLCONTEXT_H
