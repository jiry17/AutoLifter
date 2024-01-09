//
// Created by pro on 2020/11/20.
//

#include "basic/config.h"
#include "autolifter/lifting_problem.h"
#include "basic/semantics_factory.h"
#include "basic/util.h"
#include <glog/logging.h>

namespace {
    Data _runProgram(Program* program, const Data& data) {
        if (dynamic_cast<ParamSemantics*>(program->semantics)) {
            return data;
        }
        DataList sub_expr;
        for (auto* sub_program: program->sub_list) {
            auto sub_result = _runProgram(sub_program, data);
            if (sub_result.getType() == TNONE) return std::move(sub_result);
            sub_expr.emplace_back(std::move(sub_result));
        }
        if (sub_expr.size() == program->semantics->inp_type_list.size()) {
            return program->semantics->run(std::move(sub_expr), nullptr);
        } else {
            return semantics::curry(program->semantics, sub_expr);
        }
    }

    Data runProgram(Program* program, const Data& data) {
        auto result = _runProgram(program, data);
        if (result.getType() == TNONE) {
            if (program->semantics->oup_type == TINT) return config::KDefaultValue;
            else return new BoolValue(false);
        }
        return std::move(result);
    }
}

TaskType::TaskType(const std::vector<Type> &_const_types, int _arity, Type _ds_type, Program *_op):
    const_types(_const_types), arity(_arity), op(_op), ds_type(_ds_type) {
    const_num = int(const_types.size());
}


DataList Task::getConstant(const Example &example) {
    DataList result;
    for (int i = 0; i < task_type->const_num; ++i) result.push_back(example[i]);
    return result;
}

DataList Task::executeProgramOnInput(Program* program, const Example &example) {
    DataList result;
    for (int i = 0; i < task_type->arity; ++i) {
        result.push_back(runProgram(program, example[i + task_type->const_num]));
    }
    return result;
}

Data Task::executeTargetAfterOp(const Example &example) {
    auto mid = task_type->op->run(example);
    return runProgram(target, mid);
}

bool Task::evaluate(Program *program, const std::pair<Example *, Example *> &example) {
    return getConstant(*example.first) != getConstant(*example.second) ||
           executeProgramOnInput(program, *example.first) != executeProgramOnInput(program, *example.second) ||
           executeTargetAfterOp(*example.first) == executeTargetAfterOp(*example.second);
}

bool Task::evaluate(const std::vector<Program *>& program_list, const std::pair<Example*, Example*>& example) {
    if (getConstant(*example.first) != getConstant(*example.second)) return true;
    if (executeTargetAfterOp(*example.first) == executeTargetAfterOp(*example.second)) return true;
    for (auto* program: program_list) {
        if (executeProgramOnInput(program, *example.first) != executeProgramOnInput(program, *example.second)) return true;
    }
    return false;
}

DataList Task::execute(const std::vector<Program *> &program_list, const Example &example) {
    DataList result;
    for (auto* program: program_list) {
        for (auto& v: executeProgramOnInput(program, example)) {
            result.push_back(v);
        }
    }
    return result;
}

std::vector<DataList> * Task::getCacheId(Program *program) {
    std::string feature = program->toString();
    if (cache_map.count(feature)) return &evaluate_cache[cache_map[feature]];
    return nullptr;
}

void Task::acquireMoreExample(int target_num) {
    example_space->acquireMoreExamples(target_num);
}

void Task::extendCache(Program *program, int num) {
    std::string feature = program->toString();
#ifdef DEBUG
    assert(cache_map.count(feature));
#endif
    int where = cache_map[feature];
    if (evaluate_cache[where].size() < num) {
        for (int i = evaluate_cache[where].size(); i < num; ++i) {
            DataList result = executeProgramOnInput(program, *example_space->input_space[i]);
            evaluate_cache[where].push_back(std::move(result));
        }
    }
}

void Task::insertCache(Program *program, std::vector<DataList>&& data) {
#ifdef DEBUG
    assert(data.size() <= example_space->input_space.size());
#endif
    std::string feature = program->toString();
    int where = evaluate_cache.size();
    cache_map[feature] = where;
    evaluate_cache.emplace_back(data);
    program_cache.push_back(program);
}

bool Task::checkEquivalence(Program *program_1, Program *program_2) {
    std::string feature_1 = program_1->toString();
    std::string feature_2 = program_2->toString();
    if (cache_map.count(feature_1) == 0) {
        insertCache(program_1);
    }
    if (cache_map.count(feature_2) == 0) {
        insertCache(program_2);
    }
    auto& result_1 = evaluate_cache[cache_map[feature_1]];
    auto& result_2 = evaluate_cache[cache_map[feature_2]];
    for (int i = 0; i < std::min(result_1.size(), result_2.size()); ++i) {
        if (result_1[i] != result_2[i]) {
            return false;
        }
    }
    return true;
}

void Task::insertCache(Program *program) {
    auto feature = program->toString();
#ifdef DEBUG
    assert(cache_map.count(feature) == 0);
#endif
    int where = int(evaluate_cache.size()), n = int(example_space->input_space.size());
    cache_map[feature] = where;
    evaluate_cache.emplace_back(n);
    for (int i = 0 ; i < n; ++i) {
        evaluate_cache[where][i] = executeProgramOnInput(program, *example_space->input_space[i]);
    }
}

ExternalExtraSemantics::ExternalExtraSemantics(Semantics *semantics, bool _is_main_only): is_main_only(_is_main_only) {
    assert(semantics->oup_type == TINT);
    for (auto& inp_type: semantics->inp_type_list) {
        assert(inp_type == TINT);
    }
    auto* s = new NonTerminal(util::getDefaultSymbolForType(TINT), TINT);
    std::vector<NonTerminal*> param_symbol;
    for (int i = 0; i < semantics->inp_type_list.size(); ++i) {
        param_symbol.push_back(s);
    }
    s->rule_list.push_back(new Rule(semantics, param_symbol));
    nt_list.push_back(s);
}

namespace {
    Rule* rewriteRule(Rule* rule, std::map<std::string, NonTerminal*>& name_map) {
        std::vector<NonTerminal*> param_list;
        for (auto* symbol: rule->param_list) {
            param_list.push_back(name_map[symbol->name]);
        }
        return new Rule(rule->semantics, param_list);
    }
}

void ExternalExtraSemantics::insertExternalExtraSemantics(Grammar *grammar) const {
    std::map<std::string, NonTerminal*> name_map;
    for (auto* symbol: grammar->symbol_list) {
        name_map[symbol->name] = symbol;
    }
    for (auto* symbol: nt_list) {
        if (!name_map.count(symbol->name)) {
            auto* new_symbol = new NonTerminal(symbol->name, symbol->type);
            name_map[symbol->name] = new_symbol;
            grammar->symbol_list.push_back(new_symbol);
        }
    }
    for (auto* symbol: nt_list) {
        auto* start_symbol = name_map[symbol->name];
        for (auto* rule: symbol->rule_list) {
            std::vector<NonTerminal*> param_list;
            for (auto* param_symbol: rule->param_list) {
                param_list.push_back(name_map[param_symbol->name]);
            }
            start_symbol->rule_list.push_back(new Rule(rule->semantics, param_list));
        }
    }
}

namespace {
    bool _isSameRule(Rule* r_1, Rule* r_2) {
        if (r_1->semantics->name != r_2->semantics->name) return false;
        assert(r_1->param_list.size() == r_2->param_list.size());
        for (int i = 0; i < r_1->param_list.size(); ++i) {
            if (r_1->param_list[i]->name != r_2->param_list[i]->name) return false;
        }
        return true;
    }

    void _addRule(NonTerminal* nt, Rule* rule) {
        for (auto* r: nt->rule_list) {
            if (_isSameRule(r, rule)) return;
        }
        nt->rule_list.push_back(nullptr);
        for (int i = int(nt->rule_list.size()) - 1; i >= 0; --i) {
            nt->rule_list[i] = nt->rule_list[i - 1];
        }
        nt->rule_list[0] = rule;
    }
}

void ExtraSemantics::insertExtraSemantics(Grammar *grammar) const {
    /*LOG(INFO) << "Insert extra " << nt_list.size();
    auto* extra_grammar = new Grammar(nt_list[0], nt_list);
    extra_grammar->print();*/
    std::map<std::string, NonTerminal*> nt_map;
    for (auto* symbol: grammar->symbol_list) {
        nt_map[symbol->name] = symbol;
    }
    for (auto* symbol: nt_list) {
        if (!nt_map.count(symbol->name)) {
            auto* new_symbol = new NonTerminal(symbol->name, symbol->type);
            nt_map[symbol->name] = new_symbol;
            grammar->symbol_list.push_back(new_symbol);
        }
    }
    for (auto* symbol: nt_list) {
        for (auto* rule: symbol->rule_list) {
            _addRule(nt_map[symbol->name], rewriteRule(rule, nt_map));
        }
    }
}

void ExtraSemantics::initWithName(Semantics *semantics, std::string oup_name, const std::vector<std::string> &inp_name) {
    std::map<std::string, NonTerminal*> nt_map;
    auto* start = new NonTerminal(oup_name, semantics->oup_type);
    nt_map[oup_name] = start;
    std::vector<NonTerminal*> param_list;
    for (int i = 0; i < inp_name.size(); ++i) {
        if (nt_map.count(inp_name[i])) {
            assert(nt_map[inp_name[i]]->type == semantics->inp_type_list[i]);
        } else {
            nt_map[inp_name[i]] = new NonTerminal(inp_name[i], semantics->inp_type_list[i]);
        }
        param_list.push_back(nt_map[inp_name[i]]);
    }
    for (const auto& nt_info: nt_map) {
        nt_list.push_back(nt_info.second);
    }
    start->rule_list.push_back(new Rule(semantics, param_list));
}

ExtraSemantics::ExtraSemantics(Semantics *semantics, std::string oup_name, const std::vector<std::string> &inp_name) {
    initWithName(semantics, oup_name, inp_name);
}

ExtraSemantics::ExtraSemantics(const std::vector<NonTerminal *> &_nt_list): nt_list(_nt_list) {}

ExtraSemantics::ExtraSemantics(Semantics *_semantics) {
    std::string oup_name = util::getDefaultSymbolForType(_semantics->oup_type);
    std::vector<std::string> inp_name_list;
    for (auto& inp_type: _semantics->inp_type_list) {
        inp_name_list.push_back(util::getDefaultSymbolForType(inp_type));
    }
    initWithName(_semantics, oup_name, inp_name_list);
}

void Task::filterOutExternalSemantics() {
    int now = 0;
    for (auto extra: extra_sygus_semantics) {
        if (!extra.is_main_only)  {
            extra_sygus_semantics[now++] = extra;
        }
    }
    extra_sygus_semantics.resize(now);
}