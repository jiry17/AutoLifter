//
// Created by pro on 2021/5/5.
//

#include "autolifter/external_comb_solver.h"
#include "basic/util.h"
#include <glog/logging.h>
#include <unordered_set>

namespace {
    std::unordered_set<std::string> example_set;

    Program* _replaceParam(Program* program, int param_id) {
        auto* ps = dynamic_cast<ParamSemantics*>(program->semantics);
        if (ps) {
            assert(ps->getId() == 0);
            return new Program({}, new ParamSemantics(param_id, ps->getType()));
        }
        std::vector<Program*> sub_programs;
        for (auto* sub: program->sub_list) {
            sub_programs.push_back(_replaceParam(sub, param_id));
        }
        return new Program(sub_programs, program->semantics);
    }
}

namespace {
    std::string _getConstName(int id, int tot) {
        if (tot == 1) return "a";
        return "c" + std::to_string(id);
    }

    std::string _getInputName(int id, int tot) {
        if (tot == 1) return "x";
        if (tot == 2) return id == 0 ? "l" : "r";
        return "x" + std::to_string(id) + "@";
    }
}

ExternalCombSolver::ExternalCombSolver(Task *_task, const std::vector<Program *> &lifting_list,
                                       Program *_target, std::vector<ExampleInfo>&& cache, const std::vector<int>& id_list):
    task(_task), program_list(lifting_list), target(_target), client_solver(new PolyGen(task)) {
    client_solver->clearCache();

    int const_num = task->task_type->const_num, list_num = task->task_type->arity;

    for (int i = 0; i < const_num; ++i) {
        auto* param_program = new Program({}, new ParamSemantics(i, TINT));
        param_list.push_back(param_program);
        name_list.push_back(_getConstName(i, const_num));
    }

    // TODO: Use a better way to decide the number of examples;

    int example_num = std::min(cache.size(), task->example_space->input_space.size());

    std::vector<std::vector<DataList>::iterator> iterator_list;
    for (int i = 0; i < program_list.size(); ++i) {
        auto* program = program_list[i];
        auto id = id_list[i];
        for (int j = 0; j < list_num; ++j) {
            auto inp_id = j + const_num;
            param_list.push_back(_replaceParam(program, inp_id));
            name_list.push_back(_getInputName(j, list_num) + std::to_string(id));
        }
        auto* inp_cache = task->getCacheId(program);
        example_num = std::min(example_num, int(inp_cache->size()));
        iterator_list.push_back(inp_cache->begin());
    }

    example_set.clear();
    int ind = 0;
    for (int example_pos = 0; example_pos < example_num; ++example_pos) {
        auto& example = task->example_space->input_space[example_pos];
        std::string example_feature;
        for (int i = 0; i < const_num; ++i) example_feature += example->at(i).toString() + "|";
        for (auto& iterator: iterator_list) {
            example_feature += util::dataList2String(*iterator);
        }
        if (example_set.find(example_feature) == example_set.end()) {
            example_set.insert(example_feature);
            //std::cout << example_feature << " " << param_list.size() << std::endl;
            DataList inp(param_list.size());
            for (int i = 0; i < const_num; ++i) inp[i] = example->at(i);
            int num = const_num;
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
}

MergeInfo ExternalCombSolver::synthesize() {
    auto* merged_program = client_solver->synthesis(external_example_space);
    return {merged_program, param_list, name_list};
}
