//
// Created by pro on 2020/12/2.
//

#ifndef CPP_COMPLETE_SOLVER_H
#define CPP_COMPLETE_SOLVER_H

#include "solver.h"
#include "external_comb_solver.h"

class LiftingSolver {
public:
    std::vector<Task*> task_list;
    std::vector<Program*> lifting;
    std::vector<std::vector<MergeInfo>> merge_infos;
    virtual void synthesis() = 0;
    void printSummary();
    LiftingSolver(const std::vector<Task*>& _task_list): task_list(_task_list) {}
};

class CompleteSolver: public LiftingSolver {
public:
    SolverBuilder *solver_builder;
    ExternalSolverBuilder* external_builder;
    CompleteSolver(std::vector<Task*> _task_list, SolverBuilder* _builder, ExternalSolverBuilder* _external_builder):
        LiftingSolver(_task_list), solver_builder(_builder), external_builder(_external_builder) {}
    virtual void synthesis();
};

#endif //CPP_COMPLETE_SOLVER_H
