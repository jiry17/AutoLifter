//
// Created by pro on 2024/1/3.
//
#include "exp/paradigm.h"
#include "basic/config.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

LSP2Paradigm::LSP2Paradigm() {
    cbmc_template_path = config::KSourcePath + "resource/paradigm/lsp2-cbmc-template.cpp";
    runnable_template_path = config::KSourcePath + "resource/paradigm/lsp2-runnable-template.cpp";
}

std::vector<TaskType *> LSP2Paradigm::getTaskTypes() const {
    auto op1 = new Program({util::buildParamProgram(1, TLIST), util::buildParamProgram(0, TINT)},
                          semantics::getSemanticsFromName("append"));
    auto* task1 = new TaskType({TINT}, 1, TLIST, op1);
    auto op2 = new Program({util::buildParamProgram(1, TLIST)}, semantics::getSemanticsFromName("tail"));
    auto* task2 = new TaskType({TINT}, 1, TLIST, op2);
    return {task1, task2};
}

std::string LSP2Paradigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto sp_program = util::readFile(runnable_template_path);
    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "info.x" + name.substr(1);
    };

    std::vector<std::string> append_component_list, concat_component_list;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        append_component_list.push_back(util::synthesisResult2CString(solver->merge_infos[i][0], change_name));
        concat_component_list.push_back(util::synthesisResult2CString(solver->merge_infos[i][1], change_name));
    }
    sp_program = util::replaceToken(sp_program, "PushCaseString", util::mergeComponent(append_component_list));
    sp_program = util::replaceToken(sp_program, "PopCaseString", util::mergeComponent(concat_component_list));

    std::vector<std::string> zero_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init_zero = util::synthesisInitProgram(lifting_component, 0, solver->task_list[0]);
        zero_program_list.push_back(util::program2CString(init_zero, {}));
    }
    sp_program = util::replaceToken(sp_program, "ZeroCaseString", util::mergeComponent(zero_program_list));
    sp_program = util::replaceToken(sp_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));

    return sp_program;
}

void LSP2Paradigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto* task = solver->task_list[0];
    std::string result = util::readFile(cbmc_template_path);
    auto info = util::readBenchmarkFile(source_path);
    for (auto& [token, s]: info) result = util::replaceToken(result, token, s);
    result = util::replaceDefaultTokens(result, task, solver->lifting.size());

    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "info.x" + name.substr(1);
    };

    std::string append_merge_string, tail_merge_string;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto append_component = util::synthesisResult2CString(solver->merge_infos[i][0], change_name);
        auto tail_component = util::synthesisResult2CString(solver->merge_infos[i][1], change_name);
        append_merge_string += "int x" + std::to_string(i) + " = " + append_component + ";\n";
        tail_merge_string += "int x" + std::to_string(i) + " = " + tail_component + ";\n";
    }
    std::string last_line = "info = (result){";
    for (int i = 0; i < solver->lifting.size(); ++i) {
        if (i) last_line += ","; last_line += "x" + std::to_string(i);
    }
    last_line += "};";
    append_merge_string += last_line; tail_merge_string += last_line;

    result = util::replaceToken(result, "PushCaseString", append_merge_string);
    result = util::replaceToken(result, "PopCaseString", tail_merge_string);

    std::vector<std::string> zero_init_program;
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 0, task);
        zero_init_program.push_back(util::program2CString(init, {}));
    }
    result = util::replaceToken(result, "ZeroCaseString", util::mergeComponent(zero_init_program));

    util::verifyViaCBMC(result, 10);
}