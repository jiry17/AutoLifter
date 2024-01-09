//
// Created by pro on 2024/1/3.
//
#include "exp/paradigm.h"
#include "basic/config.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

SegmentTreeParadigm::SegmentTreeParadigm(const std::string &_init_tag, const std::string &_tag_operator, Semantics* _tag_applier):
    init_tag(_init_tag), tag_operator(_tag_operator), tag_applier(_tag_applier) {
    cbmc_template_path = config::KSourcePath + "resource/paradigm/segment-tree-cbmc-template.cpp";
    runnable_template_path = config::KSourcePath + "resource/paradigm/segment-tree-runnable-template.cpp";
}

std::vector<TaskType *> SegmentTreeParadigm::getTaskTypes() const {
    auto* op1 = new Program({util::buildParamProgram(0, TLIST), util::buildParamProgram(1, TLIST)},
                            semantics::getSemanticsFromName("++"));
    auto* task1 = new TaskType({}, 2, TLIST, op1);
    auto* op2 = new Program({util::buildParamProgram(0, TINT), util::buildParamProgram(1, TLIST)}, tag_applier);
    auto* task2 = new TaskType({TINT}, 1, TLIST, op2);
    return {task1, task2};
}

std::string SegmentTreeParadigm::buildRunnableProgram(LiftingSolver *solver) const {
    auto ds_program = util::readFile(runnable_template_path);
    ds_program = util::replaceToken(ds_program, "DefaultTag", init_tag);
    ds_program = util::replaceToken(ds_program, "UpdateTagString", tag_operator);
    auto merge_change_name = [](const std::string &name) -> std::string {
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        LOG(FATAL) << "Unknown param name";
    };
    auto update_change_name = [](const std::string& name) -> std::string {
        if (name[0] == 'a') return name;
        return "res.x" + name.substr(1);
    };

    std::vector<std::string> merge_components, update_components;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        merge_components.push_back(util::synthesisResult2CString(solver->merge_infos[i][0], merge_change_name));
        update_components.push_back(util::synthesisResult2CString(solver->merge_infos[i][1], update_change_name));
    }
    ds_program = util::replaceToken(ds_program, "MergeString", util::mergeComponent(merge_components));
    ds_program = util::replaceToken(ds_program, "UpdateString", util::mergeComponent(update_components));

    std::vector<std::string> one_program_list;
    for (auto& lifting_component: solver->lifting) {
        auto init_one = util::synthesisInitProgram(lifting_component, 1, solver->task_list[1]);
        one_program_list.push_back(util::program2CString(init_one, {"a"}));
    }
    ds_program = util::replaceToken(ds_program, "OneCaseString", util::mergeComponent(one_program_list));
    ds_program = util::replaceToken(ds_program, "ResultDefineString", util::buildStructureDef(solver->lifting.size()));
    ds_program = util::replaceToken(ds_program, "KINF", std::to_string(config::KDefaultValue));

    return ds_program;
}

void SegmentTreeParadigm::verifyViaCBMC(LiftingSolver *solver, const std::string &source_path) const {
    auto *task = solver->task_list[0];
    auto result = util::readFile(cbmc_template_path);
    auto info = util::readBenchmarkFile(source_path);
    for (auto& [token, s]: info) result = util::replaceToken(result, token, s);
    result = util::replaceToken(result, "DefaultTag", init_tag);
    result = util::replaceToken(result, "UpdateTagString", tag_operator);
    result = util::replaceDefaultTokens(result, task, solver->lifting.size());

    auto merge_change_name = [](const std::string &name) -> std::string {
        if (name[0] == 'l') return "lres.x" + name.substr(1);
        if (name[0] == 'r') return "rres.x" + name.substr(1);
        LOG(FATAL) << "Unknown param name";
    };
    auto update_change_name = [](const std::string& name) -> std::string {
        if (name[0] == 'a') return name;
        return "res.x" + name.substr(1);
    };

    std::string merge_string, update_string;
    for (int i = 0; i < solver->lifting.size(); ++i) {
        merge_string += "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(solver->merge_infos[i][0], merge_change_name) + ";\n";
        update_string += "int x" + std::to_string(i) + " = " + util::synthesisResult2CString(solver->merge_infos[i][1], update_change_name) + ";\n";
    }
    std::string suffix = "result res_info = (result){";
    for (int i = 0; i < solver->lifting.size(); ++i) {
        if (i) suffix += ","; suffix += "x" + std::to_string(i);
    }
    suffix += "};";
    merge_string += suffix; update_string += suffix;
    result = util::replaceToken(result, "MergeString", merge_string);
    result = util::replaceToken(result, "UpdateString", update_string);

    std::vector<std::string> one_init_program;
    for (auto &lifting_component: solver->lifting) {
        auto init = util::synthesisInitProgram(lifting_component, 1, solver->task_list[1]);
        one_init_program.push_back(util::program2CString(init, {"a"}));
    }
    result = util::replaceToken(result, "OneCaseString", util::mergeComponent(one_init_program));

    util::verifyViaCBMC(result, 6, false);
}