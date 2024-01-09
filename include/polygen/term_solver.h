//
// Created by pro on 2021/5/5.
//

#ifndef CPP_TERM_SOLVER_H
#define CPP_TERM_SOLVER_H

#include "basic/grammar.h"
#include "basic/program.h"
#include "autolifter/lifting_problem.h"
#include "gurobi_c++.h"

class TermConfig {
public:
    std::vector<int> assignment;
    int cover_num, size;
    std::vector<PointExample*> example_list;
    int operator < (const TermConfig& k1) {
        return cover_num < k1.cover_num;
    }
    TermConfig(std::vector<int>&& _assignment, int _cover_num, std::vector<PointExample*>&& _example_list):
            assignment(_assignment), cover_num(_cover_num), size(0), example_list(std::move(_example_list)) {
        for (int v: assignment) size += abs(v);
    }
};

class TermSolver {
    GRBEnv env;
    int k_limit;
    Task* task;
    std::vector<PointExample*> buildExtendedExamples(const std::vector<Program*>& atom_list, const std::vector<PointExample *> &example_list);
    std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list, int N, int K);
    std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list, int N);
public:

    TermSolver(Task* _task, int _k_limit = -1): env(GRBEnv(true)), k_limit(_k_limit), task(_task) {
        env.set("LogFile", "gurobi.log");
        env.start();
    }
    void clearCache();
    std::vector<Program*> getTerms(const std::vector<PointExample*>& example_list);
};


#endif //CPP_TERM_SOLVER_H
