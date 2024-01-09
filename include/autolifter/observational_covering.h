//
// Created by pro on 2021/1/20.
//

#ifndef CPP_OBSERVATIONAL_COVERING_H
#define CPP_OBSERVATIONAL_COVERING_H

#include "solver.h"
#include <queue>

typedef unsigned long long HASH;

struct EnumerateInfo {
    int l_id, r_id;
    HASH h;
    std::vector<int> ind_list;
    Bitset info;
    EnumerateInfo(int _l_id, int _r_id, HASH _h, const std::vector<int> &_ind_list, const Bitset& _info):
            l_id(_l_id), r_id(_r_id), h(_h), ind_list(_ind_list), info(_info) {
    }
};

class UncoveredInfoList {
public:
    std::vector<EnumerateInfo*> info_list;
    int num;
    void clear() {num = 0;}
    UncoveredInfoList(): info_list(100), num(0) {}
    bool isAdd(EnumerateInfo* x);
    int add(EnumerateInfo* x);
};

class ObservationalCoveringAuxSolver: public Solver {
public:
    void recordRes(const ProgramList& res, int turn_num, int vis_f, int syntax_num);
    void getMoreComponents();
    void addCounterExample(std::pair<Example*, Example*> counter_example);
    void initNewProgram();
    bool isAddExpression(int k, EnumerateInfo* info);
    std::pair<EnumerateInfo*, EnumerateInfo*> addExpression(int k, EnumerateInfo* info);
    HASH getComponentHash(int k);
    std::pair<Example*, Example*> verifyOccam(const std::vector<Program *> & program_list);
    void insertInput(Example* example);
    std::vector<Program*> getProgramList(EnumerateInfo* info);

    std::pair<EnumerateInfo*, EnumerateInfo*> getNextComposition(int k);
    std::pair<EnumerateInfo*, EnumerateInfo*> recoverResult(EnumerateInfo* info);
    std::vector<UncoveredInfoList> maximal_list;
    UncoveredInfoList global;
    std::vector<Bitset> program_info;
    std::vector<Program*> synthesisFromExample();
    std::vector<std::pair<int, int>> last;
    std::vector<Program*> program_space;
    std::vector<std::vector<EnumerateInfo*>> info_list;
    std::vector<std::pair<Example*, Example*>> example_list;

    std::unordered_set<unsigned long long> maximal_set;
    std::vector<HASH> hash_pool;
    std::map<std::string, Example*> inp_pool;

    int next_component_id = 0;
    const int t = 2;
    Grammar* grammar;
    ObservationalCoveringAuxSolver(Task* _task);
    virtual std::vector<Program *> synthesis();
};

class EnumerateBasedBuilder: public SolverBuilder {
public:
    virtual Solver* build(Task* task) {
        return new ObservationalCoveringAuxSolver(task);
    }
};


#endif //CPP_OBSERVATIONAL_COVERING_H
