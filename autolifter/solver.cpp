//
// Created by jiruy on 2020/11/25.
//

#include "basic/config.h"
#include "autolifter/solver.h"
#include "basic/enumerator.h"
#include "basic/recorder.h"
#include "basic/semantics_factory.h"
#include "basic/util.h"
#include "glog/logging.h"

#include <bitset>
#include <unordered_map>

namespace {
    std::unordered_map<std::string, int> data_map;

    void collectTempSemantics(Program* program, std::vector<Semantics*>& result) {
        auto* semantics = dynamic_cast<AnonymousSemantics*>(program->semantics);
        if (semantics) {
            result.push_back(semantics);
        }
        for (auto* sub: program->sub_list) {
            collectTempSemantics(sub, result);
        }
    }

    std::vector<Semantics*> collectTempSemantics(Program* program) {
        std::vector<Semantics*> result;
        collectTempSemantics(program, result);
        return result;
    }

    bool checkDefault(const Data& data) {
        if (data.getType() == TINT && data.getInt() == config::KDefaultValue) {
            return true;
        }
        return false;
    }
}

std::pair<Example*, Example*> Solver::verify(const std::vector<Program *> & program_list, bool is_using_existing) {
    data_map.clear();
    std::vector<Program*> complete_program_list = program_list;
    if (is_using_existing) {
        for (auto *program: task->existing_lifting) {
            complete_program_list.push_back(program);
        }
    } else {
        complete_program_list.push_back(task->target);
    }


    int total = 0, pos = start_verify_pos;
    std::vector<int> is_cached;
    std::vector<std::vector<DataList>*> cache_list;
    std::vector<std::vector<DataList>> temp;

    for (auto* program: complete_program_list) {
        auto* cache = task->getCacheId(program);
        if (cache) {
            is_cached.push_back(-1);
            cache_list.push_back(cache);
            task->extendCache(program, example_num);
        } else {
            is_cached.push_back(temp.size());
            temp.emplace_back();
            cache_list.push_back(nullptr);
        }
    }
    for (int i = 0; i < complete_program_list.size(); ++i) {
        if (is_cached[i] != -1) {
            cache_list[i] = &temp[is_cached[i]];
        }
    }

    auto verify = [&](int pos)->std::pair<Example*, Example*>{
        auto& example_info = example_structure[pos];
        std::string current_feature = example_info.feature + "&";
        for (int i = 0; i < complete_program_list.size(); ++i) {
            if (is_cached[i] == -1) {
                current_feature += util::dataList2String((*cache_list[i])[pos]);
            } else {
                DataList result = task->executeProgramOnInput(complete_program_list[i], *example_info.example);
                (*cache_list[i]).push_back(result);
                current_feature += util::dataList2String(result);
            }
        }
        auto iterator = data_map.find(current_feature);
        if (iterator == data_map.end()) {
            data_map[current_feature] = pos;
        } else {
            int pre_pos = (*iterator).second;
            if (example_info.info != example_structure[pre_pos].info) {
                start_verify_pos = pos;
                return std::make_pair(example_info.example, example_structure[pre_pos].example);
            }
        }
        return std::make_pair(nullptr, nullptr);
    };

    while (total < example_num) {
        total += 1;
        auto verify_result = verify(pos);
        if (verify_result.first) return verify_result;
        pos = (pos + 1) % int(example_num);
    }
    int pre_size = example_num;
    std::vector<DataList> tmp(pre_size);
    for (int i = 0; i < complete_program_list.size(); ++i) {
        if (is_cached[i] != -1) {
            for (int j = 0; j < pre_size; ++j) {
                tmp[pos] = (*cache_list[i])[j];
                pos = (pos + 1) % pre_size;
            }
            *cache_list[i] = tmp;
        }
    }
    acquireMoreExample();
    for (int i = 0; i < complete_program_list.size(); ++i) {
        if (is_cached[i] == -1) {
            task->extendCache(complete_program_list[i], example_num);
        }
    }

    for (int i = pre_size; i < example_num; ++i) {
        auto verify_result = verify(i);
        if (verify_result.first) return verify_result;
    }

    for (int i = 0; i < complete_program_list.size(); ++i) {
        if (is_cached[i] != -1) {
            task->insertCache(complete_program_list[i], std::move(*cache_list[i]));
        }
    }
    return {nullptr, nullptr};
}

void Solver::acquireMoreExample(int target_num) {
    int pre_size = example_num;
    if (target_num == -1) target_num = example_num * (1 + task->example_space->c.increase_rate);
    if (target_num < example_num) return;
    example_num = target_num;
    if (target_num > task->example_space->input_space.size())
        task->acquireMoreExample(target_num);
    for (int i = pre_size; i < example_num; ++i) {
        auto example = task->example_space->input_space[i];
        auto inp = task->executeProgramOnInput(task->target, *example);
        auto oup = task->executeTargetAfterOp(*example);
        std::string feature = util::dataList2String(inp) + util::dataList2String(task->getConstant(*example));
        example_structure.push_back({feature, oup, example});
    }
}

Solver::Solver(Task *_task): task(_task), example_num(0) {
    start_verify_pos = 0;
    acquireMoreExample(task->example_space->c.init_size);
}
