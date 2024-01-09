//
// Created by jiruy on 2020/11/24.
//


#include "exp/benchmark.h"
#include "basic/config.h"
#include "basic/semantics_factory.h"
#include "basic/util.h"
#include <glog/logging.h>
#include <unordered_set>
#include <regex>

Task * benchmark::defaultBuilder(const std::string& name, TaskType* task_type, const std::string &target_name, const SemanticsFunction& semantics,
        const ExampleSpaceConfig &c, const std::vector<ExtraSemantics> &extra_list,
        const std::vector<ExternalExtraSemantics> &extra_sygus_list) {
    std::vector<Type> type_list = task_type->const_types;
    for (int i = 0; i < task_type->arity; ++i) type_list.push_back(task_type->ds_type);
    auto* example_space = example_space::buildExample(type_list, c);
    auto* param = new Program({}, new ParamSemantics(0, TLIST));
    auto* target = new Program({param}, new AnonymousSemantics(semantics, {TLIST}, TINT, target_name));
    auto* task = new Task(name, task_type, target, example_space, extra_list, extra_sygus_list);
    return task;
}

BenchmarkInfo benchmark::getBenchmark(const std::string &type, const std::string &name) {
    if (type == "dac") {
        return getDivideAndConquer(name);
    } else if (type == "single-pass") {
        return getSinglePass(name);
    } else if (type == "longest-segment") {
        return getLongestSegment(name);
    } else if (type == "segment-tree") {
        return getSegmentTree(name);
    }
    LOG(FATAL) << "Unknown benchmark type " << type;
}