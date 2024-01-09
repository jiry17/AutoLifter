//
// Created by pro on 2020/11/23.
//

#include "basic/enumerator.h"
#include "basic/util.h"
#include "glog/logging.h"
#include <unordered_map>

namespace {

    void _splitSize(int pos, int rem, const std::vector<std::vector<int>>& pool, std::vector<int>& scheme, std::vector<std::vector<int>>& result) {
        if (pos == pool.size()) {
            if (rem == 0) {
                result.push_back(scheme);
            }
            return;
        }
        for (int value: pool[pos]) {
            if (value <= rem) {
                scheme.push_back(value);
                _splitSize(pos + 1, rem - value, pool, scheme, result);
                scheme.pop_back();
            }
        }
    }

    std::vector<std::vector<int>> splitSize(int tot, const std::vector<std::vector<int>>& pool) {
        std::vector<std::vector<int>> result;
        std::vector<int> scheme;
        _splitSize(0, tot, pool, scheme, result);
        return result;
    }

    void _mergeProgram(int pos, Rule* rule, std::vector<ProgramList>& candidate, ProgramList& sub_list, ProgramList& result) {
        if (pos == candidate.size()) {
            result.push_back(new Program(sub_list, rule->semantics));
            return;
        }
        for (auto* program: candidate[pos]) {
            sub_list.push_back(program);
            _mergeProgram(pos + 1, rule, candidate, sub_list, result);
            sub_list.pop_back();
        }
    }

    ProgramList mergeProgram(Rule* rule, std::vector<ProgramList>& candidate) {
        ProgramList result, sub_program;
        _mergeProgram(0, rule, candidate, sub_program, result);
        return result;
    }
}

FTA::FTA(Grammar *_grammar, const std::vector<Data> &_input_space): grammar(_grammar){
    for (auto& inp: _input_space) {
        input_space.push_back({inp});
    }
    for (int i = 0; i < input_space.size(); ++i) {
        param_space.push_back(new ParamInfo(input_space[i]));
    }
    grammar->indexSymbols();
    for (auto* symbol: grammar->symbol_list) {
        storage_list.emplace_back();
    }
    storage_list[0].emplace_back();
    current_size = 0;
    current_symbol = grammar->symbol_list[0];
    rule_id = 0;
    split_id = 0;
}

void FTA::nextTmp() {
    if (!sub_program_ids.empty()) {
        bool flag = 0;
        auto& nt_list = current_symbol->rule_list[rule_id]->param_list;
        for (int i = 0; i < sub_program_ids.size(); ++i) {
            if (sub_program_ids[i] + 1 < storage_list[nt_list[i]->id][size_split_list[split_id][i]].size()) {
                ++sub_program_ids[i];
                for (int j = 0; j < i; ++j) sub_program_ids[j] = 0;
                flag = 1;
            }
        }
        if (flag) return;
    }
    if (split_id + 1 < size_split_list.size()) {
        ++split_id;
        for (auto& ind: sub_program_ids) ind = 0;
        return;
    }
    while (1) {
        ++rule_id;
        if (rule_id >= current_symbol->rule_list.size()) {
            rule_id = 0;
            if (current_symbol->id + 1 >= grammar->symbol_list.size()) {
                current_size += 1;
                current_symbol = grammar->symbol_list[0];
            } else {
                current_symbol = grammar->symbol_list[current_symbol->id + 1];
            }
            storage_list[current_symbol->id].emplace_back();
        }
        std::vector<std::vector<int>> pool;
        auto& nt_list = current_symbol->rule_list[rule_id]->param_list;
        for (auto* symbol: nt_list) {
            std::vector<int> choice;
            for (int i = 1; i < current_size; ++i) {
                if (!storage_list[symbol->id][i].empty()) {
                    choice.push_back(i);
                }
            }
            pool.push_back(std::move(choice));
        }
        size_split_list = splitSize(current_size - 1, pool);
        if (size_split_list.empty()) continue;
        split_id = 0;
        sub_program_ids.resize(nt_list.size());
        for (auto& x: sub_program_ids) x = 0;
        return;
    }
}

Program* FTA::nextProgram(bool is_non_duplicate) {
    for (int _ = 1; _ <= KMaxAttempt; ++_) {
        // std::cout << current_size << " " << current_symbol->name << std::endl;
        nextTmp();
        std::vector<ProgramInfo *> sub_list;
        auto *rule = current_symbol->rule_list[rule_id];
        auto &nt_list = rule->param_list;
        for (int i = 0; i < size_split_list[split_id].size(); ++i) {
            int symbol_id = nt_list[i]->id, size = size_split_list[split_id][i];
            assert(sub_program_ids[i] < storage_list[symbol_id][size].size());
            sub_list.push_back(&storage_list[symbol_id][size][sub_program_ids[i]]);
        }
        DataList new_result;
        for (int i = 0; i < input_space.size(); ++i) {
            DataList inp;
            for (auto *info: sub_list) {
                inp.push_back(info->feature[i]);
            }
            if (inp.size() < rule->semantics->inp_type_list.size()) {
                new_result.push_back(semantics::curry(rule->semantics, inp));
            } else {
                new_result.push_back(rule->semantics->run(std::move(inp), param_space[i]));
            }
        }
        if (current_symbol->type != TSEMANTICS) {
            std::string feature = current_symbol->name + "@" + util::dataList2String(new_result);
            if (cache.find(feature) == cache.end()) {
                cache.insert(feature);
            } else if (current_symbol->id == grammar->start_symbol->id && !is_non_duplicate) {
                return nullptr;
            } else {
                continue;
            }
        } else {
            for (auto& data: new_result) {
                auto* semantics_value = dynamic_cast<SemanticsValue*>(data.value);
                semantics_value->is_anonymous = false;
            }
        }
        std::vector<Program *> sub_program_list;
        for (auto *info: sub_list) {
            sub_program_list.push_back(info->program->copy());
        }
        auto *program = new Program(sub_program_list, rule->semantics);
        storage_list[current_symbol->id][current_size].emplace_back(program, std::move(new_result));
        if (current_symbol->id == grammar->start_symbol->id) {
            return program;
        }
    }
    return nullptr;
}

// TODO: delete intermediate programs
std::vector<Program *> Enumerator::enumerateProgram(int size_limit) {
    std::vector<Program*> result;
    std::vector<ProgramStorage> storage_list;
    grammar->indexSymbols();
    // grammar->print();
    // for (auto* symbol: grammar->symbol_list) std::cout << "(" << symbol->name << ", " << symbol->id << ") ";
    // std::cout << std::endl;
    for (auto* symbol: grammar->symbol_list) {
        storage_list.push_back({{}});
    }
    for (int size = 1; size <= size_upper_bound ; ++size) {
        // grammar->print();
        for (auto* symbol: grammar->symbol_list) {
            storage_list[symbol->id].emplace_back();
            for (auto* rule: symbol->rule_list) {
                std::vector<std::vector<int>> pool;
                for (auto* param_symbol: rule->param_list) {
                    int param_id = param_symbol->id;
                    std::vector<int> choice;
                    for (int i = 1; i < size; ++i) {
                        if (!storage_list[param_id][i].empty()) {
                            choice.push_back(i);
                        }
                    }
                    pool.push_back(choice);
                }
                auto split_list = splitSize(size - 1, pool);
                for (auto& split: split_list) {
                    std::vector<ProgramList> candidate;
                    for (int i = 0; i < split.size(); ++i) {
                        candidate.emplace_back(storage_list[rule->param_list[i]->id][split[i]]);
                    }
                    for (auto* program: mergeProgram(rule, candidate)) {
                        storage_list[symbol->id][size].push_back(program);
                        if (symbol->name == grammar->start_symbol->name && v->verify(program)) {
                            result.push_back(program);
                            if (result.size() >= size_limit) {
                                return result;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

bool OEEnumerator::isInsertProgram(Program *p, NonTerminal* nt) {
    if (p->semantics->oup_type == TSEMANTICS) return true;
    DataList feature;
    for (auto& inp: inp_list) {
        feature.emplace_back(p->run({inp}));
    }
    auto feature_str = nt->name + "@" + util::dataList2String(feature);
    if (feature_pool.find(feature_str) == feature_pool.end()) {
        feature_pool.insert(feature_str);
        return true;
    }
    return false;
}

Program * OEEnumerator::synthesisProgram() {
    std::vector<ProgramStorage> storage_list;
    grammar->indexSymbols();
    for (auto* symbol: grammar->symbol_list) {
        storage_list.push_back({{}});
    }
    for (int size = 1;; ++size) {
        for (auto* symbol: grammar->symbol_list) {
            storage_list[symbol->id].emplace_back();
            for (auto* rule: symbol->rule_list) {
                std::vector<std::vector<int>> pool;
                for (auto* param_symbol: rule->param_list) {
                    int param_id = param_symbol->id;
                    std::vector<int> choice;
                    for (int i = 1; i < size; ++i) {
                        if (!storage_list[param_id][i].empty()) {
                            choice.push_back(i);
                        }
                    }
                    pool.push_back(choice);
                }
                auto split_list = splitSize(size - 1, pool);
                for (auto& split: split_list) {
                    std::vector<ProgramList> candidate;
                    for (int i = 0; i < split.size(); ++i) {
                        candidate.emplace_back(storage_list[rule->param_list[i]->id][split[i]]);
                    }
                    for (auto* program: mergeProgram(rule, candidate)) {
                        if (isInsertProgram(program, symbol)) {
                            storage_list[symbol->id][size].push_back(program);
                            if (symbol->name == grammar->start_symbol->name && v->verify(program))
                                return program;
                        }
                    }
                }
            }
        }
    }
}