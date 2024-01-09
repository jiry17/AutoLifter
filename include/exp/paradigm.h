//
// Created by pro on 2024/1/3.
//

#ifndef CPP_PARADIGM_H
#define CPP_PARADIGM_H

#include "autolifter/lifting_problem.h"
#include "autolifter/complete_solver.h"

class Paradigm {
public:
    virtual std::vector<TaskType*> getTaskTypes() const = 0;
    virtual std::string buildRunnableProgram(LiftingSolver* solver) const = 0;
};

class CBMCParadigm: public Paradigm {
public:
    virtual void verifyViaCBMC(LiftingSolver* solver, const std::string& source_path) const = 0;
};

#define DefineParadigm(name) \
class name ## Paradigm: public CBMCParadigm {     \
public:                      \
    std::string runnable_template_path, cbmc_template_path;                    \
    name ## Paradigm(); \
    virtual std::vector<TaskType*> getTaskTypes() const; \
    virtual void verifyViaCBMC(LiftingSolver* solver, const std::string& source_path) const; \
    virtual std::string buildRunnableProgram(LiftingSolver* solver) const; \
}

DefineParadigm(DivideAndConquer);
DefineParadigm(SinglePass);
DefineParadigm(LSP1);
DefineParadigm(LSP2);

class LSP3Paradigm: public CBMCParadigm {
public:
    std::string runnable_template_path, cbmc_template_path;
    std::string cmp;
    LSP3Paradigm(const std::string& _cmp);
    virtual std::vector<TaskType*> getTaskTypes() const;
    virtual void verifyViaCBMC(LiftingSolver* solver, const std::string& source_path) const;
    virtual std::string buildRunnableProgram(LiftingSolver* solver) const;
};

class SegmentTreeParadigm: public CBMCParadigm {
public:
    std::string runnable_template_path, cbmc_template_path, init_tag, tag_operator;
    Semantics* tag_applier;
    SegmentTreeParadigm(const std::string& _init_tag, const std::string& _tag_operator, Semantics* _tag_applier);
    virtual std::vector<TaskType*> getTaskTypes() const;
    virtual void verifyViaCBMC(LiftingSolver* solver, const std::string& source_path) const;
    virtual std::string buildRunnableProgram(LiftingSolver* solver) const;
};

namespace util {
    std::string buildStructureDef(int n);
    std::string replaceToken(const std::string& original, const std::string& token, const std::string& s);
    std::string readFile(const std::string& path);
    std::unordered_map<std::string, std::string> readBenchmarkFile(const std::string& path);
    Program* synthesisInitProgram(Program* lifting, int size, Task* task);
    std::string replaceDefaultTokens(const std::string& original, Task* task, int aux_num);
    std::string program2CString(Program* program, const std::vector<std::string>& param_names);
    std::string synthesisResult2CString(const MergeInfo& info, const std::function<std::string(const std::string&)>& name_map);
    std::string mergeComponent(const std::vector<std::string>& component_list);

    void verifyViaCBMC(const std::string& program, int unwind_num = -1, bool is_unwind_check = true);
}

#endif //CPP_PARADIGM_H
