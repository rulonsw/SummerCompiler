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
    class IfContext {
        int maxDepth = -1;
        std::vector<int> ifIdAtDepth;
        std::vector<int> numElsesAtDepth;
    public:
        IfContext();
        int getMaxDepth();
        int getIfLabelAtDepth(int i);
        int getNumElseLabelsAtDepth(int i);
        int pushElseAtDepth(int i);
        void popElseAtDepth(int i);
        int pushIfAtDepth(int i);

        void deepen();
    };
}



#endif //SUMMERCOMPILER_CTRLCONTEXT_H
