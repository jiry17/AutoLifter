//
// Created by jiruy on 2020/11/24.
//


#include "benchmark.h"
#include "config.h"
#include "semantics_factory.h"
#include "util.h"
#include <glog/logging.h>
#include <unordered_set>
#include <regex>

bool benchmark::KIsSinglePass = false;

Task * benchmark::defaultBuilder(const std::string &type, const std::string &target_name, const SemanticsFunction& semantics,
        const ExampleSpaceConfig &c, const std::vector<ExtraSemantics> &extra_list,
        const std::vector<ExternalExtraSemantics> &extra_sygus_list, const std::map<std::string, Program*>& extra_info) {
    assert(type != "op");
    auto* task_type = task::getTaskType(type);
    std::vector<Type> type_list;
    for (auto& variable_info: task_type->variable_list) {
        type_list.push_back(variable_info.second);
    }
    auto* example_space = example_space::buildExample(type_list, c);
    auto* param = new Program({}, new ParamSemantics(0, TLIST));
    auto* target = new Program({param}, new AnonymousSemantics(semantics, {TLIST}, TINT, target_name));
    auto* task = new Task(type, task_type, target, example_space, extra_info, extra_list, extra_sygus_list);
    return task;
}

std::vector<Task *> benchmark::getBenchmark(const std::string &type, const std::string &name) {
    if (type == "dad") {
        return {getDivideAndConquer(name)};
    } else if (type == "seg") {
        return getLongestSegment(name);
    } else if (type == "ds") {
        return getSegmentTree(name);
    } else if (type == "listr") {
        return {getListR(name)};
    }
}