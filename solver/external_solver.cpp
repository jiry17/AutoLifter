//
// Created by pro on 2021/5/5.
//

#include "external_solver.h"
#include "util.h"
#include <glog/logging.h>
#include <unordered_set>

namespace {
    std::unordered_set<std::string> example_set;
}

ExternalSolver::ExternalSolver(Task *_task, const std::vector<Program *> &lifting_list,
                               Program *_target, std::vector<ExampleInfo>&& cache, const std::vector<int>& id_list):
    task(_task), program_list(lifting_list), target(_target), client_solver(new PolyGen(task)) {
    //LOG(INFO) << "Current Lifting:";
    //for (auto* program: program_list) LOG(INFO) << program;
    //for (auto* program: program_list) LOG(INFO) << program->toString();

    client_solver->clearCache();

    // TODO: a better way to get name
    std::map<Type, std::vector<std::string> > name_map = {
            {TINT, {"a"}}, {TLIST, {"l", "r"}}
    };

    int constant_pos = -1;
    for (auto* condition: task->task_type->condition) {
        int c = 0;
        if (condition->base == ConstraintInfo::NONE) {
            int pos = condition->param_list[0];
            auto* param_program = new Program({}, new ParamSemantics(condition->param_list[0], TINT));
            param_list.push_back(param_program);
            name_list.push_back(name_map[TINT][c]);
            ++c;
            assert(constant_pos == -1);
            constant_pos = pos;
        }
    }

    // TODO: Use a better way to decide the number of examples;

    int example_num = std::min(cache.size(), task->example_space->input_space.size());

    std::vector<std::vector<DataList>::iterator> iterator_list;
    for (int i = 0; i < program_list.size(); ++i) {
        auto* program = program_list[i];
        LOG(INFO) << "program: " << program->toString();
        auto id = id_list[i];
        int c = 0;
        for (auto* condition: task->task_type->condition) {
            if (condition->base == ConstraintInfo::PROGRAM) {
                auto* replaced_program = task->getReplacedProgram(program, condition);
                param_list.push_back(replaced_program);
                name_list.push_back(name_map[TLIST][c] + std::to_string(id));
                ++c;
            }
        }
        auto* inp_cache = task->getCacheId(program);
        assert(inp_cache);
        example_num = std::min(example_num, int(inp_cache->size()));
        iterator_list.push_back(inp_cache->begin());
    }

    example_set.clear();
    int ind = 0;
    for (int example_pos = 0; example_pos < example_num; ++example_pos) {
        auto& example = task->example_space->input_space[example_pos];
        std::string example_feature = constant_pos == -1 ? "" : (*example)[constant_pos].toString() + "|";
        for (auto& iterator: iterator_list) {
            example_feature += util::dataList2String(*iterator);
        }
        if (example_set.find(example_feature) == example_set.end()) {
            example_set.insert(example_feature);
            //std::cout << example_feature << " " << param_list.size() << std::endl;
            DataList inp(param_list.size());
            int num = 0;
            if (constant_pos != -1) {
                // std::cout << num << " " << (*example)[constant_pos].toString() << " " << inp[num].value << std::endl;
                inp[num++] = (*example)[constant_pos];
            }
            for (auto& iterator: iterator_list) {
                for (auto& item: *iterator) {
                    inp[num++] = item;
                }
            }

            external_example_space.push_back(new PointExample(std::move(inp), std::move(cache[ind].info)));
        }
        ++ind;
        for (auto& iterator: iterator_list) {
            ++iterator;
        }
    }
    // std::cout << example_num << " " << external_example_space.size() << std::endl;
    // LOG(INFO) << "finished " << external_example_space.size();
}

MergeInfo ExternalSolver::synthesize() {
    auto* merged_program = client_solver->synthesis(external_example_space);
    return {merged_program, param_list, name_list};
}