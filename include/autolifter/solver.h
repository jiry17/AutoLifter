//
// Created by jiruy on 2020/11/25.
//

#ifndef CPP_SOLVER_H
#define CPP_SOLVER_H


#include "lifting_problem.h"
#include "basic/bitset.h"
#include "basic/enumerator.h"

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

struct CompositionInfo {
    int l_id, r_id;
    std::vector<Program*> program;
    Bitset info;
    CompositionInfo(int _l_id, int _r_id, const std::vector<Program*> &_program, const Bitset& _info):
        l_id(_l_id), r_id(_r_id), program(_program), info(_info) {
    }
    ~CompositionInfo() {
        for (auto* p: program) delete p;
    }
};

#endif //CPP_SOLVER_H
