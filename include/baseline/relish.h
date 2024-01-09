//
// Created by pro on 2022/11/8.
//

#ifndef CPP_HFTA_COMPLETE_SOLVER_H
#define CPP_HFTA_COMPLETE_SOLVER_H

#include "autolifter/complete_solver.h"

class Relish: public LiftingSolver {
public:
    virtual void synthesis();
    Relish(const std::vector<Task*>& _task_list): LiftingSolver(_task_list) {}
    virtual ~Relish() = default;
};

namespace hfta {
    std::pair<std::vector<Program*>, Program*> HFTASynthesis(Task* task);
}


#endif //CPP_HFTA_COMPLETE_SOLVER_H
