//
// Created by jiruy on 2020/11/25.
//

#ifndef CPP_SOLVER_H
#define CPP_SOLVER_H


#include "task.h"
#include "bitset.h"
#include "enumerator.h"

#include <unordered_set>

class Solver {
public:
    Task* task;
    std::vector<ExampleInfo> example_structure;
    int start_verify_pos;
    int example_num;
    virtual std::vector<Program*> synthesis() = 0;
    std::pair<Example*, Example*> verify(const std::vector<Program*>&, bool);
    Solver(Task* _task);
    void acquireMoreExample(int target_num = -1);
    virtual ~Solver() = default;
};

class SolverBuilder {
public:
    virtual Solver* build(Task* task) = 0;
};

#endif //CPP_SOLVER_H
