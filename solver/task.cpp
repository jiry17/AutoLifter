//
// Created by pro on 2020/11/20.
//

#include "config.h"
#include "task.h"
#include "semantics_factory.h"
#include "util.h"
#include <glog/logging.h>

namespace {
    TaskType* list_lifting = nullptr;
    TaskType* listr_lifting = nullptr;
    TaskType* list_tail_lifting = nullptr;
    TaskType* operator_lifting = nullptr;
    TaskType* tree_lifting = nullptr;
    TaskType* segment_lifting = nullptr;

    bool isSameRule(Rule* r_1, Rule* r_2) {
        if (r_1->semantics->name != r_2->semantics->name) return false;
        assert(r_1->param_list.size() == r_2->param_list.size());
        for (int i = 0; i < r_1->param_list.size(); ++i) {
            if (r_1->param_list[i]->name != r_2->param_list[i]->name) return false;
        }
        return true;
    }

    void addRule(NonTerminal* nt, Rule* rule) {
        for (auto* r: nt->rule_list) {
            if (isSameRule(r, rule)) return;
        }
        nt->rule_list.push_back(nullptr);
        for (int i = int(nt->rule_list.size()) - 1; i >= 0; --i) {
            nt->rule_list[i] = nt->rule_list[i - 1];
        }
        nt->rule_list[0] = rule;
    }

    void initListLifting() {
        auto* constant = new Program({}, new ParamSemantics(0, TINT));
        auto* cons = new Program({constant, new Program({}, new ParamSemantics(1, TLIST))}, semantics::getSemanticsFromName("cons"));
        list_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::NONE,
                                    {new ProgramReplace(constant)}, {0}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1})},
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET,
                                    {new ProgramReplace(cons)}, {0, 1})},
                {{"c", TINT}, {"x", TLIST}}
                );
    }

    void initListRLifting() {
        auto* constant = new Program({}, new ParamSemantics(0, TINT));
        auto* cons = new Program({new Program({}, new ParamSemantics(1, TLIST)), constant}, semantics::getSemanticsFromName("append"));
        listr_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::NONE,
                                    {new ProgramReplace(constant)}, {0}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1})},
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET,
                                    {new ProgramReplace(cons)}, {0, 1})},
                {{"c", TINT}, {"x", TLIST}}
        );
    }

    void initListTailLifting() {
        auto* constant = new Program({}, new ParamSemantics(0, TINT));
        auto* list = new Program({}, new ParamSemantics(0, TLIST));
        auto* tail = new Program({list}, semantics::getSemanticsFromName("tail"));
        list_tail_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::NONE, {new ProgramReplace(constant)}, {0}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1})},
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET, {new ProgramReplace(tail)}, {1})},
                {{"c", TINT}, {"x", TLIST}}
                );
    }

    void initMaximumSegmentLifting() {
        auto* constant = new Program({}, new ParamSemantics(0, TINT));
        auto* l = new Program({}, new ParamSemantics(1, TLIST));
        auto* r = new Program({}, new ParamSemantics(2, TLIST));
        auto connect_func = [](DataList&& inp, GlobalInfo* global_info)->Data {
            LIST result;
            int a = inp[0].getInt();
            LIST l = inp[1].moveList(), r = inp[2].moveList();
            l.push_back(a);
            for (auto& x: r) l.push_back(x);
            return new ListValue(std::move(l));
        };
        auto* connect = new Program({constant, l, r},
                new AnonymousSemantics(connect_func, {TINT, TLIST, TLIST}, TLIST, "connect"));
        segment_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::NONE, {new ProgramReplace(constant)}, {0}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {2})},
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET, {new ProgramReplace(connect)}, {0, 1, 2})},
                {{"a", TINT}, {"l", TLIST}, {"r", TLIST}}
        );
    }

    void initTreeLifting() {
        auto* list_0 = new Program({}, new ParamSemantics(0, TLIST));
        auto* list_1 = new Program({}, new ParamSemantics(1, TLIST));
        auto* append = new Program({list_0, list_1}, semantics::getSemanticsFromName("++"));
        tree_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {0}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1})},
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET,
                                    {new ProgramReplace(append)}, {0, 1})},
                {{"l", TLIST}, {"r", TLIST}}
                );
    }

    void initOperatorLifting() {
        auto* constant = new Program({}, new ParamSemantics(0, TINT));
        operator_lifting = new TaskType (
                {new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::PROGRAM, {}, {1}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::NONE,
                                    {new ProgramReplace(constant)}, {0})},
                {new ConstraintInfo(ConstraintInfo::EqType::UNEQUAL, ConstraintInfo::BaseType::TARGET, {}, {1}),
                 new ConstraintInfo(ConstraintInfo::EqType::EQUAL, ConstraintInfo::BaseType::TARGET,
                                    {new OperatorReplace()}, {0, 1})},
                {{"c", TINT}, {"x", TLIST}}
                );
    }

    void initTaskTypes() {
        initListLifting();
        initTreeLifting();
        initListRLifting();
        initListTailLifting();
        initOperatorLifting();
        initMaximumSegmentLifting();
    }

    Data _RunProgram(Program* program, const Data& data) {
        if (dynamic_cast<ParamSemantics*>(program->semantics)) {
            return data;
        }
        DataList sub_expr;
        for (auto* sub_program: program->sub_list) {
            auto sub_result = _RunProgram(sub_program, data);
            if (sub_result.getType() == TNONE) return std::move(sub_result);
            sub_expr.emplace_back(std::move(sub_result));
        }
        if (sub_expr.size() == program->semantics->inp_type_list.size()) {
            return program->semantics->run(std::move(sub_expr), nullptr);
        } else {
            return semantics::curry(program->semantics, sub_expr);
        }
    }

    Data RunProgram(Program* program, const Data& data) {
        auto result = _RunProgram(program, data);
        if (result.getType() == TNONE) {
            if (program->semantics->oup_type == TINT) return config::KDefaultValue;
            else return new BoolValue(false);
        }
        return std::move(result);
    }

    void rewriteParam(Program* program, std::vector<int> param_list) {
        auto* param_semantics = dynamic_cast<ParamSemantics*>(program->semantics);
        if (param_semantics) {
            int id = param_semantics->getId();
#ifdef DEBUG
            assert(id < param_list.size());
#endif
            program->semantics = new ParamSemantics(param_list[id], param_semantics->getType());
        }
        for (auto* sub_program: program->sub_list) {
            rewriteParam(sub_program, param_list);
        }
    }
}

TaskType * task::getTaskType(std::string task_name) {
    if (!list_lifting) initTaskTypes();
    if (task_name == "list") return list_lifting;
    else if (task_name == "tree") return tree_lifting;
    else if (task_name == "operator") return operator_lifting;
    else if (task_name == "segment") return segment_lifting;
    else if (task_name == "listr") return listr_lifting;
    else if (task_name == "tail") return list_tail_lifting;
    assert(0);
}

Program * ProgramReplace::performReplace(Program *program, Task *task_info) {
    if (program->sub_list.size() == 0) {
        auto* param_semantics = dynamic_cast<ParamSemantics*>(program->semantics);
        if (param_semantics) return replacement->copy();
        return program->copy();
    }
    std::vector<Program*> sub_list;
    for (auto* sub_program: program->sub_list) {
        sub_list.push_back(performReplace(sub_program, task_info));
    }
    return new Program(sub_list, program->semantics);
}

Program * OperatorReplace::performReplace(Program *program, Task *task_info) {
    if (program->sub_list.size() == 0) {
        auto* param_semantics = dynamic_cast<ParamSemantics*>(program->semantics);
        if (param_semantics) {
            assert(task_info->auxiliary_map.count("operator"));
            return task_info->auxiliary_map["operator"]->copy();
        }
        return program->copy();
    }
    std::vector<Program*> sub_list;
    for (auto* sub_program: program->sub_list) {
        sub_list.push_back(performReplace(sub_program, task_info));
    }
    return new Program(sub_list, program->semantics);
}

Data OperatorReplace::run(const DataList &inp, Task *task_info) {
#ifdef DEBUG
    assert(task_info->auxiliary_map.count("operator"));
#endif
    return task_info->auxiliary_map["operator"]->run(inp);
}

/*Data Task::execute(ConstraintInfo *constraint, Program *program, const Example &example, bool is_ignore_target) {
    Program* current_program = nullptr;
    switch (constraint->base) {
        case ConstraintInfo::BaseType::TARGET:
            current_program = is_ignore_target ? program->copy() : target->copy(); break;
        case ConstraintInfo::BaseType::PROGRAM:
            current_program = program->copy(); break;
        case ConstraintInfo::BaseType::NONE:
            current_program = new Program({}, new ParamSemantics(0, TLIST)); break;
    }
    for (auto* replace: constraint->replace_list) {
        auto* new_program = replace->performReplace(current_program, this);
        delete current_program;
        current_program = new_program;
    }
    Data now = current_program->run(constraint->getPartialExample(example));
    delete current_program;
    return std::move(now);
}*/

Program* Task::getReplacedProgram(Program *program, ConstraintInfo *constraint) {
    Program* current_program = program->copy();
    assert(constraint->base == ConstraintInfo::BaseType::PROGRAM);
    for (auto* replace: constraint->replace_list) {
        auto* new_program = replace->performReplace(current_program, this);
        delete current_program;
        current_program = new_program;
    }
    rewriteParam(current_program, constraint->param_list);
    return current_program;
}

Data Task::execute(ConstraintInfo *constraint, Program *program, const Example &example, bool is_ignore_target) {
    assert(constraint->replace_list.size() <= 1);
    auto partial_example = constraint->getPartialExample(example);
    Data current;
    if (constraint->replace_list.empty()) {
        current = std::move(partial_example[0]);
    } else {
        current = constraint->replace_list[0]->run(partial_example, this);
    }
    switch (constraint->base) {
        case ConstraintInfo::BaseType::TARGET:
            return RunProgram(is_ignore_target ? program : target, current);
        case ConstraintInfo::BaseType::PROGRAM:
            return RunProgram(program, current);
        case ConstraintInfo::BaseType::NONE:
            return std::move(current);
    }
}

bool Task::evaluateOnConstraint(ConstraintInfo *constraint, Program *program, const std::pair<Example*, Example*>& example) {
    auto result_1 = execute(constraint, program, *example.first);
    auto result_2 = execute(constraint, program, *example.second);
    // std::cout << program->toString() << " " << datalist2String(*example.first) << " " <<datalist2String(*example.second) << std::endl;
    switch (constraint->type) {
        case ConstraintInfo::EqType::EQUAL: return result_1 == result_2;
        case ConstraintInfo::EqType::UNEQUAL: return !(result_1 == result_2);
    }
}

bool Task::evaluate(Program *program, const std::pair<Example*, Example*>& example) {
    for (auto* constraint: task_type->condition) {
        if (!evaluateOnConstraint(constraint, program, example)) return true;
    }
    for (auto* property: task_type->property) {
        if (evaluateOnConstraint(property, program, example)) return true;
    }
    return false;
}

bool Task::evaluate(std::vector<Program *> program_list, const std::pair<Example*, Example*>& example) {
    for (auto* program: program_list) {
        if (evaluate(program, example)) return true;
    }
    return false;
}

DataList Task::execute(const std::vector<Program *> &program_list, const Example &example) {
    DataList result;
    for (auto* constraint: task_type->condition) {
        if (constraint->base == ConstraintInfo::PROGRAM) {
            for (auto *program: program_list) {
                result.emplace_back(execute(constraint, program, example));
            }
        }
    }
    return std::move(result);
}

DataList Task::execute(Program *program, const Example &example) {
    DataList result;
    for (auto* constraint: task_type->condition) {
        if (constraint->base == ConstraintInfo::PROGRAM) {
            result.push_back(execute(constraint, program, example));
        }
    }
    return std::move(result);
}

std::pair<DataList, Data> Task::executeExample(const Example &example) {
    DataList same;
    Data distinct;
    int distinct_num = 0;
    for (auto* condition: task_type->condition) {
        if (condition->base == ConstraintInfo::BaseType::NONE) {
            Data result = execute(condition, nullptr, example);
            assert(condition->type == ConstraintInfo::EqType::EQUAL);
            same.emplace_back(std::move(result));
        }
    }
    for (auto* property: task_type->property) {
        assert(property->base != ConstraintInfo::BaseType::PROGRAM);
        Data result = execute(property, nullptr, example);
        if (property->type == ConstraintInfo::EqType::EQUAL) {
            distinct_num += 1;
            distinct = std::move(result);
        } else {
            same.emplace_back(std::move(result));
        }
    }
    assert(distinct_num == 1);
    return {std::move(same), std::move(distinct)};
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
            DataList result = execute(program, *example_space->input_space[i]);
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
        evaluate_cache[where][i] = execute(program, *example_space->input_space[i]);
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
            addRule( nt_map[symbol->name], rewriteRule(rule, nt_map));
        }
    }
}

void ExtraSemantics::initWithName(Semantics *semantics, std::string oup_name, const std::vector<std::string> &inp_name) {
    // std::cout << "Init extra semantics " << oup_name;
    // for (int i = 0; i < inp_name.size(); ++i) std::cout << " " << inp_name[i];
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