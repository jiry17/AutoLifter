//
// Created by pro on 2020/11/20.
//

#ifndef CPP_LIFTING_PROBLEM_H
#define CPP_LIFTING_PROBLEM_H

#include <map>
#include <unordered_map>
#include "basic/grammar.h"
#include "basic/program.h"
#include "autolifter/example_space.h"

class Task;

class TaskType {
public:
    std::vector<Type> const_types;
    int arity, const_num;
    Type ds_type;
    Program* op;
    TaskType(const std::vector<Type>& _const_types, int _arity, Type _ds_type, Program* _op);
};

class ExternalExtraSemantics {
public:
    std::vector<NonTerminal*> nt_list;
    bool is_main_only = false;
    ExternalExtraSemantics(Semantics* _semantics = nullptr, bool _is_main_only = false);
    void insertExternalExtraSemantics(Grammar* grammar) const;
};

class ExtraSemantics {
    void initWithName(Semantics* semantice, std::string oup_name, const std::vector<std::string>& inp_name);
public:
    Semantics* semantics;
    std::vector<NonTerminal*> nt_list;
    ExtraSemantics(Semantics* _semantics);
    ExtraSemantics(Semantics* semantics, std::string oup_name, const std::vector<std::string>& inp_name);
    ExtraSemantics(const std::vector<NonTerminal*>& _nt_list);
    void insertExtraSemantics(Grammar* grammar) const;
};

class Task {
public:
    TaskType* task_type;
    ExampleSpace * example_space;
    std::vector<ExtraSemantics> extra_semantics;
    std::vector<ExternalExtraSemantics> extra_sygus_semantics;
    std::vector<Program*> existing_lifting;
    Program* target;
    std::string name;

    std::vector<std::vector<DataList>> evaluate_cache;
    std::vector<Program*> program_cache;
    std::unordered_map<std::string, int> cache_map;

    Task(const std::string& _name, TaskType* _task_type, Program* _target, ExampleSpace* _example_space,
         const std::vector<ExtraSemantics>& _extra_semantics = {}, const std::vector<ExternalExtraSemantics>& _extra_sygus_semantics = {}):
         task_type(_task_type), example_space(_example_space), extra_semantics(_extra_semantics), target(_target),
         extra_sygus_semantics(_extra_sygus_semantics), name(_name) {
    }

    std::vector<DataList>* getCacheId(Program* program);
    void acquireMoreExample(int target_num = -1);
    void insertCache(Program* program, std::vector<DataList>&& data);
    Data executeTargetAfterOp(const Example& example);
    DataList executeProgramOnInput(Program* program, const Example& example);
    DataList getConstant(const Example& example);
    DataList execute(const std::vector<Program*>& program_list, const Example& example);
    bool evaluate(Program* program, const std::pair<Example*, Example*>& example);
    bool evaluate(const std::vector<Program*>& program_list, const std::pair<Example*, Example*>& example);
    bool checkEquivalence(Program* program_1, Program* program_2);
    void insertCache(Program* program);
    void extendCache(Program* program, int num);
    void filterOutExternalSemantics();
};

namespace task {
    TaskType* getTaskType(std::string task_name);
}


#endif //CPP_LIFTING_PROBLEM_H
