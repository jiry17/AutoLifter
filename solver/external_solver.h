//
// Created by pro on 2021/5/5.
//

#ifndef CPP_EXTERNAL_SOLVER_H
#define CPP_EXTERNAL_SOLVER_H

#include "program.h"
#include "example_space.h"
#include "task.h"
#include "polygen.h"

class MergeInfo {
public:
    Program* merged_program;
    std::vector<Program*> inp_list;
    std::vector<std::string> param_name;
};

class PolyGen;

class ExternalSolver {
public:
    std::vector<PointExample*> external_example_space;
    int verify_pos;
    Task* task;
    std::vector<Program*> program_list, param_list;
    Program* target;
    PolyGen* client_solver;
    std::vector<std::string> name_list;
    ExternalSolver(Task* _task, const std::vector<Program*>& lifting_list,
                   Program* _target, std::vector<ExampleInfo> &&cache, const std::vector<int>& id_list);
    MergeInfo synthesize();
    ~ExternalSolver() {
        for (auto* example: external_example_space) {
            delete example;
        }
    }
};

class ExternalSolverBuilder {
public:
    ExternalSolver* build(Task* _task, const std::vector<Program*>& lifting_list, Program* _target,
            std::vector<ExampleInfo>&& cache, const std::vector<int>& id_list) {
        return new ExternalSolver(_task, lifting_list, _target, std::move(cache), id_list);
    }
};

#endif //CPP_EXTERNAL_SOLVER_H
