//
// Created by pro on 2020/11/23.
//

#ifndef CPP_ENUMERATOR_H
#define CPP_ENUMERATOR_H

#include "grammar.h"
#include "program.h"
#include <unordered_set>

#include <unordered_set>

class Verifier {
public:
    virtual bool verify(Program* program) = 0;
};

class DefaultVerifier: public Verifier {
public:
    virtual bool verify(Program*) {return true;}
};

typedef std::vector<Program*> ProgramList;
typedef std::vector<ProgramList> ProgramStorage;

class Enumerator{
public:
    Grammar* grammar;
    Verifier* v;
    int size_upper_bound = 1e9;

    Enumerator(Grammar* _grammar): grammar(_grammar), v(new DefaultVerifier()) {}
    Enumerator(Grammar* _grammar, int _size_upper_bound):
        grammar(_grammar), v(new DefaultVerifier), size_upper_bound(_size_upper_bound) {}
    Enumerator(Grammar* _grammar, Verifier* _v): grammar(_grammar), v(_v) {}
    std::vector<Program*> enumerateProgram(int num_limit);
};

class OEEnumerator {
    bool isInsertProgram(Program* p, NonTerminal* nt);
public:
    Grammar* grammar;
    Verifier* v;
    DataList inp_list;
    std::unordered_set<std::string> feature_pool;

    OEEnumerator(Grammar* _grammar, Verifier* _v, const DataList& _inp): grammar(_grammar), v(_v), inp_list(_inp) {}
    Program* synthesisProgram();
};


struct ProgramInfo {
    Program* program;
    DataList feature;
    ProgramInfo(Program* _program, DataList&& _feature): program(_program), feature(_feature) {}
};
typedef std::vector<ProgramInfo> InfoList;
typedef std::vector<InfoList> InfoStorage;

class FTA {
    std::vector<InfoStorage> storage_list;
    std::unordered_set<std::string> cache;
    int current_size, split_id, rule_id;
    NonTerminal* current_symbol;
    std::vector<int> sub_program_ids;
    std::vector<std::vector<int>> size_split_list;
    void nextTmp();
public:
    Grammar* grammar;
    std::vector<DataList> input_space;
    std::vector<ParamInfo*> param_space;
    const int KMaxAttempt = 1000;
    FTA(Grammar* _grammar, const std::vector<Data>& _input_space);
    Program* nextProgram(bool is_non_duplicate = true);
    ~FTA() {
        for (auto& storage: storage_list) {
            for (auto& info_list: storage) {
                for (auto& info: info_list) {
                    delete info.program;
                }
            }
        }
    }
};


#endif //CPP_ENUMERATOR_H
