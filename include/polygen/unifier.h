//
// Created by pro on 2021/5/5.
//

#ifndef CPP_UNIFIER_H
#define CPP_UNIFIER_H

#include "basic/program.h"
#include "basic/bitset.h"
#include "autolifter/lifting_problem.h"

struct CmpInfo {
    Program* cmp;
    Bitset P, N;
    CmpInfo(Program* _cmp, const Bitset& _P, const Bitset& _N): cmp(_cmp), P(_P), N(_N) {}
};

class Unifier {
    std::vector<CmpInfo*> excludeDuplicated(const std::vector<Program*>& program_list);
    bool verifySolvable(const std::vector<CmpInfo*>& cmp_list);
    Task* task;
public:
    std::vector<Program*> extra_list;
    std::vector<int> param_list;
    std::vector<PointExample*> P, N;
    std::vector<CmpInfo*> getCmpList(int cmp_size);
    Program* getCondition(const std::vector<PointExample*>& positive_example, const std::vector<PointExample*>& negative_example);
    Unifier(Task* _task): task(_task) {}
};


#endif //CPP_UNIFIER_H
