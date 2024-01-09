//
// Created by pro on 2021/5/5.
//

#ifndef CPP_EXTERNAL_COMB_SOLVER_H
#define CPP_EXTERNAL_COMB_SOLVER_H

#include "basic/program.h"
#include "autolifter/example_space.h"
#include "autolifter/lifting_problem.h"
#include "polygen/polygen.h"

class MergeInfo {
public:
    Program* merged_program;
    std::vector<Program*> inp_list;
    std::vector<std::string> param_name;
};

class PolyGen;

class ExternalCombSolver {
public:
    std::vector<PointExample*> external_example_space;
    int verify_pos;
    Task* task;
    std::vector<Program*> program_list, param_list;
    Program* target;
    PolyGen* client_solver;
    std::vector<std::string> name_list;
    ExternalCombSolver(Task* _task, const std::vector<Program*>& lifting_list,
                       Program* _target, std::vector<ExampleInfo> &&cache, const std::vector<int>& id_list);
    MergeInfo synthesize();
    ~ExternalCombSolver() {
        for (auto* example: external_example_space) {
            delete example;
        }
    }
};

class ExternalSolverBuilder {
public:
    ExternalCombSolver* build(Task* _task, const std::vector<Program*>& lifting_list, Program* _target,
                              std::vector<ExampleInfo>&& cache, const std::vector<int>& id_list) {
        return new ExternalCombSolver(_task, lifting_list, _target, std::move(cache), id_list);
    }
};

#endif //CPP_EXTERNAL_COMB_SOLVER_H
