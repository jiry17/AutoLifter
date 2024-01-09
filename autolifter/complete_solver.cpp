//
// Created by pro on 2020/12/2.
//

#include "autolifter/complete_solver.h"

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
}

void CompleteSolver::synthesis() {
    LOG(INFO) << "Start Synthesizing Lifting" << std::endl;

    existing_program.clear();
    lifting.clear();
    merge_infos.clear();
    lifting.push_back(task_list[0]->target);

    existing_program.insert(task_list[0]->target->toString());
    for (int i = 0; i < lifting.size(); ++i) {
        std::vector<MergeInfo> merge_info;
        for (auto* task: task_list) {
            LOG(INFO) << "Lifting " << lifting[i]->toString() << " for " << task->name << std::endl;
            recorder::recorder.pushStage("lifting");

            task->target = lifting[i];
            task->existing_lifting = lifting;
            auto *solver = solver_builder->build(task);
            auto result = solver->synthesis();

            std::vector<int> id_list;
            for (int id = 0; id < result.size(); ++id) {
                auto* new_lifting = result[id];
                std::string feature = new_lifting->toString();
                if (existing_program.find(feature) == existing_program.end()) {
                    int pre_id = -1;
                    existing_program.insert(feature);
                    for (int j = 0; j < lifting.size(); ++j) {
                        auto* existing_lift = lifting[j];
                        if (task->checkEquivalence(existing_lift, new_lifting)) {
                            result[id] = existing_lift;
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
            for (int j = 0; j < result.size(); ++j) {
                std::cout << "\t(" << j << "): " << result[j]->toString() << std::endl;
            }
            std::cout << std::endl;

            result.push_back(lifting[i]);
            id_list.push_back(i);

            // LOG(INFO) << "before external" << lifting[i]->toString() << std::endl;
            recorder::recorder.popStage();

            if (config::is_lifting_only) {
                delete solver;
                continue;
            }

            recorder::recorder.pushStage("external");
            LOG(INFO) << "Lift for " << lifting[i]->toString();
            auto* external_solver = external_builder->build(task, result, lifting[i], std::move(solver->example_structure), id_list);
            auto merge_result = external_solver->synthesize();
            merge_info.push_back(merge_result);
            std::cout << "Merging for " << lifting[i]->toString() << " in Task " << task->name << std::endl;
            std::cout << merge_result.merged_program->toString() << std::endl;
            for (int j = 0; j < merge_result.inp_list.size(); ++j) {
                std::cout << "\t(" << j << "): " << merge_result.inp_list[j]->toString() << std::endl;
            }
            recorder::recorder.popStage();
            delete solver;
            delete external_solver;
        }
        merge_infos.push_back(merge_info);
        if (i == 0) {
            for (auto* task: task_list) task->filterOutExternalSemantics();
        }
    }
    recorder::recorder.addRecord("#lifting", int(lifting.size()) - 1);
}

void LiftingSolver::printSummary() {
    std::cout << "x0: " << lifting[0]->toString() << std::endl;
    std::cout << "Liftings:" << std::endl;
    for (int i = 1; i < lifting.size(); ++i) {
        std::cout << "  x" << i << ": " << lifting[i]->toString() << std::endl;
    }
    for (auto task_id = 0; task_id < task_list.size(); ++task_id) {
        std::cout << "Combinator for " << task_list[task_id]->name << std::endl;
        for (int i = 0; i < lifting.size(); ++i) {
            std::cout << "  x" << i << ": " << printCombinator(merge_infos[i][task_id]) << std::endl;
        }
    }
}
