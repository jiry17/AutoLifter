//
// Created by pro on 2024/1/3.
//
#include "exp/paradigm.h"
#include "basic/config.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

std::vector<TaskType *> SinglePassParadigm::getTaskTypes() const {
    auto arity = 1;
    std::vector<Type> const_types = {TINT};
    Type ds_type = TLIST;
    auto op = new Program({util::buildParamProgram(1, ds_type), util::buildParamProgram(0, TINT)},
                          semantics::getSemanticsFromName("append"));
    return {new TaskType(const_types, arity, ds_type, op)};
}

SinglePassParadigm::SinglePassParadigm() {
    runnable_template_path = config::KSourcePath + "resource/paradigm/single-pass-runnable-template.cpp";
    cbmc_template_path = config::KSourcePath + "resource/paradigm/single-pass-cbmc-template.cpp";
}

std::string SinglePassParadigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto sp_program = util::readFile(runnable_template_path);

    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "res.x" + name.substr(1);
    };

    std::vector<std::string> merge_program_list;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto& merge_info = solver->merge_infos[i][0];
        merge_program_list.push_back(util::synthesisResult2CString(merge_info, change_name));
    }

    std::vector<std::string> init_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, solver->task_list[0]);
        init_program_list.push_back(util::program2CString(init, {"a"}));
    }

    sp_program = util::replaceToken(sp_program, "MergeCaseString", util::mergeComponent(merge_program_list));
    sp_program = util::replaceToken(sp_program, "InitCaseString", util::mergeComponent(init_program_list));
    sp_program = util::replaceToken(sp_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));

    return sp_program;
}

void SinglePassParadigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto* task = solver->task_list[0];
    auto result = util::readFile(cbmc_template_path);
    auto info = util::readBenchmarkFile(source_path);
    for (auto& [token, s]: info) result = util::replaceToken(result, token, s);
    result = util::replaceDefaultTokens(result, task, solver->lifting.size());

    auto change_name = [](const std::string& name) -> std::string {
        if (name == "a") return name;
        return "res.x" + name.substr(1);
    };

    std::string merge_string;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        auto& merge_info = solver->merge_infos[i][0];
        merge_string += "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(merge_info, change_name) + ";\n";
    }
    merge_string += "res = (result){";
    for (int i = 0; i < solver->lifting.size(); ++i) {
        if (i) merge_string += ","; merge_string += "x" + std::to_string(i);
    }

    std::vector<std::string> init_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, task);
        init_program_list.push_back(util::program2CString(init, {"a"}));
    }

    result = util::replaceToken(result, "MergeCaseString", merge_string + "};");
    result = util::replaceToken(result, "InitCaseString", util::mergeComponent(init_program_list));

    util::verifyViaCBMC(result);
}