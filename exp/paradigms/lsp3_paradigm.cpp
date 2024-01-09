//
// Created by pro on 2024/1/3.
//
#include "exp/paradigm.h"
#include "basic/config.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

LSP3Paradigm::LSP3Paradigm(const std::string& _cmp): cmp(_cmp) {
    cbmc_template_path = config::KSourcePath + "resource/paradigm/lsp3-cbmc-template.cpp";
    runnable_template_path = config::KSourcePath + "resource/paradigm/lsp3-runnable-template.cpp";
}

std::vector<TaskType *> LSP3Paradigm::getTaskTypes() const {
    auto cons_op = new Program({util::buildParamProgram(0, TINT), util::buildParamProgram(2, TLIST)},
                               semantics::getSemanticsFromName("cons"));
    auto op1 = new Program({util::buildParamProgram(1, TLIST), cons_op}, semantics::getSemanticsFromName("++"));
    auto* task1 = new TaskType({TINT}, 2, TLIST, op1);
    auto op2 = new Program({util::buildParamProgram(0, TINT), util::buildParamProgram(1, TLIST)},
                           semantics::getSemanticsFromName("cons"));
    auto* task2 = new TaskType({TINT}, 1, TLIST, op2);
    auto op3 = new Program({util::buildParamProgram(1, TLIST), util::buildParamProgram(0, TINT)},
                           semantics::getSemanticsFromName("append"));
    auto* task3 = new TaskType({TINT}, 1, TLIST, op3);
    return {task1, task2, task3};
}

std::string LSP3Paradigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto sp_program = util::readFile(runnable_template_path);

    auto full_name = [](const std::string &name) -> std::string {
        if (name == "a") return name;
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        LOG(FATAL) << "Unknown param name " << name;
    };
    auto cons_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "rres.x" + name.substr(1);
    };
    auto append_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "lres.x" + name.substr(1);
    };

    std::vector<std::string> merge_components, cons_components, append_components;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto merge_component = util::synthesisResult2CString(solver->merge_infos[i][0], full_name);
        merge_components.push_back(merge_component);
        auto cons_component = util::synthesisResult2CString(solver->merge_infos[i][1], cons_name);
        cons_components.push_back(cons_component);
        auto append_component = util::synthesisResult2CString(solver->merge_infos[i][2], append_name);
        append_components.push_back(append_component);
    }
    sp_program = util::replaceToken(sp_program, "FullMergeString", util::mergeComponent(merge_components));
    sp_program = util::replaceToken(sp_program, "LeftMergeString", util::mergeComponent(cons_components));
    sp_program = util::replaceToken(sp_program, "RightMergeString", util::mergeComponent(append_components));

    std::vector<std::string> zero_program_list, one_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init_one = util::synthesisInitProgram(lifting_component, 1, solver->task_list[0]);
        one_program_list.push_back(util::program2CString(init_one, {"a"}));
        auto init_zero = util::synthesisInitProgram(lifting_component, 0, solver->task_list[0]);
        zero_program_list.push_back(util::program2CString(init_zero, {}));
    }
    sp_program = util::replaceToken(sp_program, "OneCaseString", util::mergeComponent(one_program_list));
    sp_program = util::replaceToken(sp_program, "ZeroCaseString", util::mergeComponent(zero_program_list));
    sp_program = util::replaceToken(sp_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));
    sp_program = util::replaceToken(sp_program, "OperatorString", cmp);

    return sp_program;
}

void LSP3Paradigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto *task = solver->task_list[0];
    auto result = util::readFile(cbmc_template_path);
    auto info = util::readBenchmarkFile(source_path);
    for (auto& [token, s]: info) result = util::replaceToken(result, token, s);
    result = util::replaceDefaultTokens(result, task, solver->lifting.size());

    auto full_name = [](const std::string &name) -> std::string {
        if (name == "a") return name;
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        LOG(FATAL) << "Unknown param name " << name;
    };
    auto cons_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "rres.x" + name.substr(1);
    };
    auto append_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "lres.x" + name.substr(1);
    };

    std::string full_string, cons_string, append_string;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto full_component = "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(solver->merge_infos[i][0], full_name) + ";\n";
        auto cons_component = "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(solver->merge_infos[i][1], cons_name) + ";\n";
        auto append_component = "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(solver->merge_infos[i][2], append_name) + ";\n";
        full_string += full_component; cons_string += cons_component; append_string += append_component;
    }
    std::string suffix = "result res_info = (result){";
    for (int i = 0; i < solver->lifting.size(); ++i) {
        if (i) suffix += ",";
        suffix += "x" + std::to_string(i);
    }
    suffix += "};";
    full_string += suffix; cons_string += suffix; append_string += suffix;
    result = util::replaceToken(result, "FullMergeString", full_string);
    result = util::replaceToken(result, "LeftMergeString", cons_string);
    result = util::replaceToken(result, "RightMergeString", append_string);

    std::vector<std::string> zero_init_program;
    for (auto &lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 0, task);
        zero_init_program.push_back(util::program2CString(init, {}));
    }
    std::vector<std::string> one_init_program;
    for (auto &lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, task);
        one_init_program.push_back(util::program2CString(init, {"a"}));
    }
    result = util::replaceToken(result, "ZeroCaseString", util::mergeComponent(zero_init_program));
    result = util::replaceToken(result, "OneCaseString", util::mergeComponent(one_init_program));
    result = util::replaceToken(result, "OperatorString", cmp);

    util::verifyViaCBMC(result, 10);
}