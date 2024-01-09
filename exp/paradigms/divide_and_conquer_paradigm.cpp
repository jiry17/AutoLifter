//
// Created by pro on 2024/1/3.
//

#include "exp/paradigm.h"
#include "basic/util.h"
#include "basic/config.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

std::vector<TaskType *> DivideAndConquerParadigm::getTaskTypes() const {
    auto arity = 2;
    std::vector<Type> const_types; Type ds_type = TLIST;
    auto op = new Program({util::buildParamProgram(0, ds_type), util::buildParamProgram(1, ds_type)},
                          semantics::getSemanticsFromName("++"));
    return {new TaskType(const_types, arity, ds_type, op)};
}

DivideAndConquerParadigm::DivideAndConquerParadigm() {
    cbmc_template_path = config::KSourcePath + "resource/paradigm/dac-cbmc-template.cpp";
    runnable_template_path = config::KSourcePath + "resource/paradigm/dac-runnable-template.cpp";
}

void DivideAndConquerParadigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto* task = solver->task_list[0];
    auto result = util::readFile(cbmc_template_path);
    auto reference = util::readBenchmarkFile(source_path);

    for (auto& [token, s]: reference) result = util::replaceToken(result, token, s);
    result = util::replaceDefaultTokens(result, solver->task_list[0], solver->lifting.size());

    std::vector<std::string> merge_program_list;
    auto name_wrapper = [](const std::string& name) {
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        return name;
    };
    for (int i = 0; i < solver->lifting.size(); ++i) {
        merge_program_list.push_back(util::synthesisResult2CString(solver->merge_infos[i][0], name_wrapper));
    }

    std::vector<std::string> init_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, task);
        init_program_list.push_back(util::program2CString(init, {"w[l]"}));
    }

    result = util::replaceToken(result, "MergeCaseString", util::mergeComponent(merge_program_list));
    result = util::replaceToken(result, "InitCaseString", util::mergeComponent(init_program_list));

    util::verifyViaCBMC(result);
}

std::string DivideAndConquerParadigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto dac_program = util::readFile(runnable_template_path);

    auto& example_config = solver->task_list[0]->example_space->c;

    auto change_name = [](const std::string& name) -> std::string {
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        LOG(FATAL) << "Unknown name " << name;
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

    dac_program = util::replaceToken(dac_program, "MergeCaseString", util::mergeComponent(merge_program_list));
    dac_program = util::replaceToken(dac_program, "InitCaseString", util::mergeComponent(init_program_list));
    dac_program = util::replaceToken(dac_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));

    return dac_program;
}