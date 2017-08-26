//
// Created by Rulon Wood on 8/23/17.
//

#include "CtrlContext.h"

namespace RSWCOMP {
    CtrlContext::CtrlContext() {
        maxIdAtDepth.push_back(0);
        maxDepth = 0;
        depthIdStack[0].push_back(0);

    }

    int CtrlContext::getMaxDepth() {return maxDepth;}
    int CtrlContext::popIdAtDepth(int i) {
        if (i > maxDepth) throw "pop index cannot be larger than maxDepth.";
        int ret = depthIdStack[i].back();
        depthIdStack[i].pop_back();

        return ret;
    }

    int CtrlContext::pushCtrlAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";

        maxIdAtDepth[i] += 1;
        depthIdStack[i].push_back(maxIdAtDepth[i]);
        return maxIdAtDepth[i];
    }

    void CtrlContext::deepen() {
        ++maxDepth;
        maxIdAtDepth.push_back(0);
        depthIdStack[maxDepth].push_back(0);
    }

    int CtrlContext::getIdAtDepth(int i) {
        return depthIdStack[i].back();
    }

    IfContext::IfContext() {
        maxDepth = 0;
        ifIdAtDepth.push_back(0);
        numElsesAtDepth.push_back(0);
    }

    int IfContext::getNumElseLabelsAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";

        return numElsesAtDepth[i];
    }
    int IfContext::getMaxDepth() {
        return maxDepth;
    }

    int IfContext::getIfLabelAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";

        return ifIdAtDepth[i];
    }

    int IfContext::pushElseAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";


//        auto ret =numElsesAtDepth[i];
        return ++numElsesAtDepth[i];

    }
    void IfContext::popElseAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";
        numElsesAtDepth[i]--;
    }

    int IfContext::pushIfAtDepth(int i) {
        if (i > maxDepth) throw "Something terrible has happened!";
        auto ret = ifIdAtDepth[i];
        ++ifIdAtDepth[i];
        return ret;
    }

    void IfContext::deepen() {
        maxDepth +=1;
        ifIdAtDepth.push_back(0);
        numElsesAtDepth.push_back(0);
    }
}

