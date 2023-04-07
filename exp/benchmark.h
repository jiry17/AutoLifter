//
// Created by jiruy on 2020/11/24.
//

#ifndef CPP_BENCHMARK_H
#define CPP_BENCHMARK_H


#include "task.h"

typedef std::function<Task*()> TaskBuilder;
typedef std::function<std::vector<Task*>()> BatchedTaskBuilder;
namespace benchmark {
    Task* defaultBuilder(const std::string& type, const std::string& target_name, const SemanticsFunction& semantics,
            const ExampleSpaceConfig& c, const std::vector<ExtraSemantics>& extra_list={},
            const std::vector<ExternalExtraSemantics>& extra_sygus_list={}, const std::map<std::string, Program*>& extra_info={});
    std::vector<Task*> getBenchmark(const std::string& type, const std::string& name);
    Task* getDivideAndConquer(const std::string& name);
    Task* getListR(const std::string& name);
    std::vector<Task*> getLongestSegment(const std::string& name);
    std::vector<Task*> getSegmentTree(const std::string& name);
    extern bool KIsSinglePass;
}


#endif //CPP_BENCHMARK_H
