//
// Created by pro on 2022/11/8.
//

#ifndef CPP_HFTA_COMPLETE_SOLVER_H
#define CPP_HFTA_COMPLETE_SOLVER_H

#include "complete_solver.h"

class HFTACompleteSolver: public LiftingSolver {
public:
    virtual void synthesis();
    HFTACompleteSolver(const std::vector<Task*>& _task_list): LiftingSolver(_task_list) {}
    virtual ~HFTACompleteSolver() = default;
};

namespace hfta {
    std::pair<std::vector<Program*>, Program*> HFTASynthesis(Task* task);
}


#endif //CPP_HFTA_COMPLETE_SOLVER_H
