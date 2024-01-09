//
// Created by pro on 2020/12/2.
//

#include "baseline/relish.h"

#include "glog/logging.h"
#include "basic/recorder.h"
#include "basic/config.h"

#include <set>

namespace {
    std::set<std::string> existing_program;

    std::set<std::string> brackets_map= {"div", "ite"};
    std::map<std::string, std::string> name_map = {
            {"*", "*"}, {"+", "+"}, {"<", "<"}, {"<=", "<="}, {"and", "&&"}, {"or", "||"},
            {"div", "/"}, {"=", "="}, {"int", ""}
    };

    std::string _programToReadableString(Program* program, const std::vector<std::string>& name_list) {
        auto* cs = dynamic_cast<ConstSemantics*>(program->semantics);
        if (cs) return cs->value.toString();
        auto* ps = dynamic_cast<ParamSemantics*>(program->semantics);
        if (ps) return name_list[ps->getId()];
        std::vector<std::string> sub_list;
        for (auto* sub: program->sub_list) {
            sub_list.push_back(_programToReadableString(sub, name_list));
        }
        std::string now;
        bool pd = brackets_map.find(program->semantics->name) != brackets_map.end();
        if (pd) for (auto& k: sub_list) k = "(" + k + ")";
        std::string op_name = program->semantics->name;
        if (name_map.count(op_name)) op_name = name_map[op_name];
        else if (sub_list.size() == 2) op_name = " "  + op_name + " ";
        if (sub_list.size() == 2) return sub_list[0] + op_name + sub_list[1];
        else if (program->semantics->name == "int") return sub_list[0];
        else {
            auto result = op_name;
            for (auto& k: sub_list) result += " " + k;
            return result;
        }
    }

    std::string printCombinator(const MergeInfo& info) {
        return _programToReadableString(info.merged_program, info.param_name);
    }

    MergeInfo buildMergeResult(Task* task, const std::vector<int>& id_list, int target_id, Program* res) {
        std::vector<std::string> name_list;
        std::map<Type, std::vector<std::string>> name_map = {{TINT, {"a"}}, {TLIST, {"l", "r"}}};
        std::vector<Program*> param_list;
        int int_num = task->task_type->const_num, list_num = task->task_type->arity;
        for (int j = 0; j < list_num; ++j) {
            for (int id: id_list) name_list.push_back(name_map[TLIST][j] + std::to_string(id));
        }
        for (int i = 0; i < list_num; ++i) name_list.push_back(name_map[TLIST][i] + std::to_string(target_id));
        for (int i = 0; i < int_num; ++i) name_list.push_back(name_map[TINT][i]);
        return {res, {}, name_list};
    }
}

void Relish::synthesis() {
    LOG(INFO) << "Start Synthesizing Lifting" << std::endl;

    recorder::recorder.pushStage("synthesis");
    existing_program.clear();
    lifting.clear();
    merge_infos.clear();
    lifting.push_back(task_list[0]->target);

    existing_program.insert(task_list[0]->target->toString());
    for (int i = 0; i < lifting.size(); ++i) {
        std::vector<MergeInfo> merge_info;
        for (auto* task: task_list) {
            task->target = lifting[i];
            task->existing_lifting = lifting;

            auto result = hfta::HFTASynthesis(task);

            std::vector<int> id_list;
            for (int id = 0; id < result.first.size(); ++id) {
                auto* new_lifting = result.first[id];
                std::string feature = new_lifting->toString();
                if (existing_program.find(feature) == existing_program.end()) {
                    int pre_id = -1;
                    existing_program.insert(feature);
                    for (int j = 0; j < lifting.size(); ++j) {
                        auto* existing_lift = lifting[j];
                        if (task->checkEquivalence(existing_lift, new_lifting)) {
                            result.first[id] = existing_lift;
                            pre_id = j;
                            break;
                        }
                    }
                    if (pre_id != -1) {
                        id_list.push_back(pre_id);
                        continue;
                    }
                    id_list.push_back(lifting.size());
                    lifting.push_back(new_lifting);
                    LOG(INFO) << "New lifting: " << new_lifting->toString() << std::endl;
                } else {
                    for (int j = 0; j < lifting.size(); ++j) {
                        if (lifting[j]->toString() == new_lifting->toString()) {
                            id_list.push_back(j);
                            break;
                        }
                    }
                }
            }

            std::cout << "Lifting " << lifting[i]->toString() << " for " << task->name << std::endl;
            for (int j = 0; j < result.first.size(); ++j) {
                std::cout << "\t(" << j << "): " << result.first[j]->toString() << std::endl;
            }
            std::cout << std::endl;

            auto merge_result = buildMergeResult(task, id_list, i, result.second);
            merge_info.push_back(merge_result);
        }
        merge_infos.push_back(merge_info);
        if (i == 0) {
            for (auto* task: task_list) task->filterOutExternalSemantics();
        }
    }
    recorder::recorder.addRecord("#lifting", int(lifting.size()) - 1);
    recorder::recorder.popStage();
}