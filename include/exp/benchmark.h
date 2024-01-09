//
// Created by jiruy on 2020/11/24.
//

#ifndef CPP_BENCHMARK_H
#define CPP_BENCHMARK_H


#include "autolifter/lifting_problem.h"
#include "paradigm.h"

struct BenchmarkInfo {
    Paradigm* paradigm;
    std::vector<Task*> tasks;
    std::string source_path;
    std::vector<std::string> other_infos;
};

namespace benchmark {
    Task* defaultBuilder(const std::string& name, TaskType* task_type, const std::string& target_name, const SemanticsFunction& semantics,
                         const ExampleSpaceConfig& c, const std::vector<ExtraSemantics>& extra_list,
                         const std::vector<ExternalExtraSemantics>& extra_sygus_list);
    BenchmarkInfo getBenchmark(const std::string& type, const std::string& name);
    BenchmarkInfo getDivideAndConquer(const std::string& name);
    BenchmarkInfo getSinglePass(const std::string& name);
    BenchmarkInfo getLongestSegment(const std::string& name);
    BenchmarkInfo getSegmentTree(const std::string& name);
}


#endif //CPP_BENCHMARK_H
