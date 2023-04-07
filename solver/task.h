//
// Created by pro on 2020/11/20.
//

#ifndef CPP_TASK_H
#define CPP_TASK_H

#include <map>
#include <unordered_map>
#include "grammar.h"
#include "program.h"
#include "example_space.h"

class Task;

class Replace {
public:
    virtual Program* performReplace(Program* program, Task* task_info) = 0;
    virtual Data run(const DataList& inp, Task* task_info) = 0;
};

class ProgramReplace: public Replace {
public:
    Program* replacement;
    ProgramReplace(Program* _program): replacement(_program) {}
    virtual Program* performReplace(Program* program, Task* task_info);
    virtual Data run(const DataList& inp, Task* task_info) {
        return replacement->run(inp);
    }
};

class OperatorReplace: public Replace {
public:
    OperatorReplace() = default;
    virtual Program* performReplace(Program* program, Task* task_info);
    virtual Data run(const DataList& inp, Task* task_info);
};

class ConstraintInfo {
public:
    enum EqType {EQUAL, UNEQUAL};
    enum BaseType {PROGRAM, TARGET, NONE};
    EqType type; // 0 for =, 1 for !=
    BaseType base; // 0 for program, 1 for target, 2 for replacement
    std::vector<Replace*> replace_list;
    std::vector<int> param_list;
    ConstraintInfo(EqType _type, BaseType _base, const std::vector<Replace*>& _replace, const std::vector<int>& _param_list):
        type(_type), base(_base), replace_list(_replace), param_list(_param_list) {
    }
    DataList getPartialExample(const Example& example) const{
        DataList result;
        for (int pos: param_list) result.push_back(example[pos]);
        return std::move(result);
    }
};

class TaskType {
public:
    std::vector<ConstraintInfo*> condition;
    std::vector<ConstraintInfo*> property;
    std::vector<std::pair<std::string, Type>> variable_list;
    TaskType(const std::vector<ConstraintInfo*> &_condition, const std::vector<ConstraintInfo*> &_property,
            std::vector<std::pair<std::string, Type>> _variable_list):
            condition(_condition), property(_property), variable_list(_variable_list){}
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
    std::map<std::string, Program*> auxiliary_map;
    std::vector<ExtraSemantics> extra_semantics;
    std::vector<ExternalExtraSemantics> extra_sygus_semantics;
    std::vector<Program*> existing_lifting;
    Program* target;
    std::string name;

    std::vector<std::vector<DataList>> evaluate_cache;
    std::vector<Program*> program_cache;
    std::unordered_map<std::string, int> cache_map;

    Task(std::string _name, TaskType* _task_type, Program* _target, ExampleSpace* _example_space, const std::map<std::string, Program*>& _auxiliary_map = {},
         const std::vector<ExtraSemantics>& _extra_semantics = {}, const std::vector<ExternalExtraSemantics>& _extra_sygus_semantics = {}):
         task_type(_task_type), example_space(_example_space), extra_semantics(_extra_semantics), target(_target), auxiliary_map(_auxiliary_map),
         extra_sygus_semantics(_extra_sygus_semantics), name(_name) {
    }

    Program* getReplacedProgram(Program* program, ConstraintInfo* constraint);
    std::vector<DataList>* getCacheId(Program* program);
    void acquireMoreExample(int target_num = -1);
    void insertCache(Program* program, std::vector<DataList>&& data);
    std::pair<DataList, Data> executeExample(const Example& example);
    Data execute(ConstraintInfo* constraint, Program* program, const Example& example, bool is_ignore_target = false);
    DataList execute(const std::vector<Program*>& program_list, const Example& example);
    DataList execute(Program* program, const Example& example);
    bool evaluateOnConstraint(ConstraintInfo* constraint, Program* program, const std::pair<Example*, Example*>& example);
    bool evaluate(Program* program, const std::pair<Example*, Example*>& example);
    bool evaluate(std::vector<Program*> program_list, const std::pair<Example*, Example*>& example);
    bool checkEquivalence(Program* program_1, Program* program_2);
    void insertCache(Program* program);
    void extendCache(Program* program, int num);
    void filterOutExternalSemantics();
};

namespace task {
    TaskType* getTaskType(std::string task_name);
}


#endif //CPP_TASK_H
