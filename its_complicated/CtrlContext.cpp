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

    int CtrlContext::popIdAtDepth(int i) {
        if (i > maxDepth) throw ("pop index cannot be larger than maxDepth.");
        int ret = depthIdStack[i].back();
        depthIdStack[i].pop_back();

        return ret;
    }

    int CtrlContext::pushCtrlAtDepth(int i) {
        if (i > maxDepth) {
            throw("Something terrible has happened!");
        }
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
}

