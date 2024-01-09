//
// Created by pro on 2024/1/3.
//
#include "exp/paradigm.h"
#include "basic/config.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

LSP1Paradigm::LSP1Paradigm() {
    cbmc_template_path = config::KSourcePath + "resource/paradigm/lsp1-cbmc-template.cpp";
    runnable_template_path = config::KSourcePath + "resource/paradigm/lsp1-runnable-template.cpp";
}

std::vector<TaskType *> LSP1Paradigm::getTaskTypes() const {
    auto arity = 1;
    std::vector<Type> const_types = {TINT};
    Type ds_type = TLIST;
    auto op = new Program({util::buildParamProgram(1, TLIST), util::buildParamProgram(0, TINT)},
                          semantics::getSemanticsFromName("append"));
    return {new TaskType(const_types, arity, ds_type, op)};
}

std::string LSP1Paradigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto sp_program = util::readFile(runnable_template_path);

    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "info.x" + name.substr(1);
    };

    std::vector<std::string> merge_program_list;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto& merge_info = solver->merge_infos[i][0];
        merge_program_list.push_back(util::synthesisResult2CString(merge_info, change_name));
    }

    std::vector<std::string> zero_program_list, one_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init_one = util::synthesisInitProgram(lifting_component, 1, solver->task_list[0]);
        one_program_list.push_back(util::program2CString(init_one, {"a"}));
        auto init_zero = util::synthesisInitProgram(lifting_component, 0, solver->task_list[0]);
        zero_program_list.push_back(util::program2CString(init_zero, {}));
    }
    sp_program = util::replaceToken(sp_program, "MergeCaseString", util::mergeComponent(merge_program_list));
    sp_program = util::replaceToken(sp_program, "ZeroCaseString", util::mergeComponent(zero_program_list));
    sp_program = util::replaceToken(sp_program, "OneCaseString", util::mergeComponent(one_program_list));
    sp_program = util::replaceToken(sp_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));

    return sp_program;
}

void LSP1Paradigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto* task = solver->task_list[0];
    auto result = util::readFile(cbmc_template_path);
    auto info = util::readBenchmarkFile(source_path);
    for (auto& [token, s]: info) {
        result = util::replaceToken(result, token, s);
    }
    result = util::replaceDefaultTokens(result, solver->task_list[0], solver->lifting.size());

    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "info.x" + name.substr(1);
    };

    std::string merge_string;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto& merge_info = solver->merge_infos[i][0];
        merge_string += "int x" + std::to_string(i) + " = " +
                util::synthesisResult2CString(merge_info, change_name) + ";\n";
    }
    merge_string += "info = (result){";
    for (int i = 0; i < solver->lifting.size(); ++i) {
        if (i) merge_string += ","; merge_string += "x" + std::to_string(i);
    }

    std::vector<std::string> one_init_program, zero_init_program;
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, task);
        one_init_program.push_back(util::program2CString(init, {"a"}));
    }
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 0, task);
        zero_init_program.push_back(util::program2CString(init, {}));
    }

    result = util::replaceToken(result, "MergeCaseString", merge_string + "};");
    result = util::replaceToken(result, "OneCaseString", util::mergeComponent(one_init_program));
    result = util::replaceToken(result, "ZeroCaseString", util::mergeComponent(zero_init_program));

    util::verifyViaCBMC(result);
}