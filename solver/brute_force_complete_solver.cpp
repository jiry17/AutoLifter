//
// Created by pro on 2021/6/28.
//

#include "brute_force_complete_solver.h"
#include "semantics_factory.h"
#include "util.h"
#include "recorder.h"

#include <set>

namespace {
    void _decomposeProgram(Program* program, std::vector<Program*>& result) {
        std::string op = program->semantics->name;
        if (op == "[]" || op == "cons") {
            for (auto* sub_program: program->sub_list) {
                _decomposeProgram(sub_program, result);
            }
            return;
        }
        result.push_back(program);
    }

    std::vector<Program*> decomposeProgram(Program* program) {
        std::vector<Program*> result;
        _decomposeProgram(program, result);
        if (result.size() == 1 && dynamic_cast<ConstSemantics*>(result[0]->semantics)) {
            return {};
        }
        return result;
    }

    ProgramList enumeratePrograms(Grammar* g, int size) {
        auto* enumerator = new Enumerator(g);
        auto result = enumerator->enumerateProgram(size);
        delete enumerator;
        return result;
    }

    int getInpNum(Task* task, const std::vector<Program*>& prog_list) {
        int res = 0;
        for (auto* condition: task->task_type->condition) {
            if (condition->base == ConstraintInfo::NONE) {
                ++res;
            } else if (condition->base == ConstraintInfo::PROGRAM) {
                res += int(prog_list.size());
            }
        }
        return res;
    }

    DataList buildInp(Example* example, Task* task, const ProgramList& l_list) {
        DataList result;
        for (auto* cond: task->task_type->condition) {
            if (cond->base == ConstraintInfo::NONE) {
                result.push_back((*example)[cond->param_list[0]]);
            }
        }
        for (auto* l: l_list) {
            for (auto& v: task->execute(l, *example)) {
                result.push_back(v);
            }
        }
        return result;
    }

    Data buildOup(Example* example, Task* task, Program* l) {
        for (auto* p: task->task_type->property) {
            if (p->base == ConstraintInfo::TARGET && p->type == ConstraintInfo::EQUAL) {
                return task->execute(p, l, *example, true);
            }
        }
        assert(false);
    }

    bool collectCombinator(const ProgramList& c_pool, int n, const ProgramList& l_list, ProgramList& result, Task* task) {
        std::vector<DataList> inp_list;
        for (auto* l: l_list) {
            DataList oup_list;
            bool is_found = false;
            for (int c_id = 0; c_id < n; ++c_id) {
                auto* c = c_pool[c_id];
                bool is_valid = true;
                for (int i = 0; i < task->example_space->input_space.size(); ++i) {
                    auto* example = task->example_space->input_space[i];
                    if (i == inp_list.size()) {
                        inp_list.push_back(buildInp(example, task, l_list));
                    }
                    if (i == oup_list.size()) {
                        oup_list.push_back(buildOup(example, task, l));
                    }
                    if (c->run(inp_list[i]) != oup_list[i]) {
                        is_valid = false;
                        break;
                    }
                }
                if (is_valid) {
                    result.push_back(c);
                    is_found = true;
                    break;
                }
            }
            if (!is_found) return false;
        }
        return true;
    }

    std::vector<MergeInfo> buildDefaultMergeInfo(Task* task, const ProgramList& c_list, const ProgramList& l_list) {
        std::map<Type, std::vector<std::string>> name_map = {{TINT, {"a"}}, {TLIST, {"l", "r"}}};
        std::vector<std::string> name_list;
        std::vector<Program*> param_list;
        int int_num = 0;
        for (auto* c: task->task_type->condition) {
            if (c->base == ConstraintInfo::NONE) {
                name_list.push_back(name_map[TINT][int_num]);
                param_list.push_back(new Program({}, new ParamSemantics(c->param_list[0], TINT)));
                int_num++;
            }
        }
        for (int i = 0; i < l_list.size(); ++i) {
            int list_num = 0;
            for (auto *c: task->task_type->condition) {
                if (c->base == ConstraintInfo::PROGRAM) {
                    name_list.push_back(name_map[TLIST][list_num] + std::to_string(i));
                    param_list.push_back(task->getReplacedProgram(l_list[i], c));
                    list_num++;
                }
            }
        }
        std::vector<MergeInfo> result;
        for (int i = 0; i < l_list.size(); ++i) {
            result.push_back((MergeInfo){c_list[i], param_list, name_list});
        }
        return result;
    }
}

Grammar * BfLiftingSolver::buildLiftingGrammar() {
    auto *grammar = grammar::getDeepCoderDSL();
    grammar->addParam({TLIST});
    std::set<std::string> semantics_set;
    for (auto* task: task_list) {
        for (auto &extra_semantics: task->extra_semantics) {
            extra_semantics.insertExtraSemantics(grammar);
        }
    }
    auto *start = new NonTerminal("start", TLIST);
    grammar->symbol_list.push_back(start);
    grammar->start_symbol = start;
    NonTerminal *int_symbol = nullptr;
    for (auto *symbol: grammar->symbol_list) {
        if (symbol->name == "int_expr") int_symbol = symbol;
    }
    auto *single_rule = new Rule(semantics::getSemanticsFromName("[]"), {int_symbol});
    auto *cons_rule = new Rule(semantics::getSemanticsFromName("cons"), {int_symbol, start});
    start->rule_list.push_back(single_rule);
    start->rule_list.push_back(cons_rule);
    return grammar;
}

Grammar* BfLiftingSolver::buildCombinatorGrammar(int size) {
    auto* cmp = new NonTerminal("bool_expr", TBOOL);
    auto* atom = new NonTerminal("param", TINT);
    auto* def_value = new NonTerminal("default_value", TINT);
    for (int i = 0; i < size; ++i) {
        atom->rule_list.push_back(new Rule(new ParamSemantics(i, TINT), {}));
    }
    for (int i = 0; i < 2; ++i) {
        atom->rule_list.push_back(new Rule(new ConstSemantics(i), {}));
    }

    def_value->rule_list.push_back(new Rule(new ConstSemantics(config::KDefaultValue), {}));
    auto* expr = new NonTerminal("int_expr", TINT);
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("int"), {atom}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("+"), {expr, atom}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("-"), {expr, atom}));
    expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("ite"), {cmp, expr, atom}));
    cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<="), {expr, expr}));
    cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {expr, expr}));
    cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("="), {expr, def_value}));
    auto* grammar = new Grammar(expr, {cmp, atom, expr, def_value});
    for (auto* task: task_list) {
        for (auto &extra_semantics: task->extra_sygus_semantics) {
            extra_semantics.insertExternalExtraSemantics(grammar);
        }
    }
    return grammar;
}

void BfLiftingSolver::synthesis() {
    recorder::recorder.pushStage("synthesis");
    int lift_size = step_size_1;
    int combinator_size = step_size_2;
    std::map<int, ProgramList> combinator_pool;
    while (1) {
        std::vector<Program*> lifting_list = enumeratePrograms(lifting_grammar, lift_size);
        for (auto* l: lifting_list) {
            std::vector<std::vector<MergeInfo> > merge_list;
            bool is_valid = true;
            ProgramList l_list = {task_list[0]->target};
            for (auto* l_prog: decomposeProgram(l)) {
                l_list.push_back(l_prog);
            }
            for (auto* task: task_list) {
                int inp_num = getInpNum(task, l_list);
                if (combinator_pool[inp_num].size() < combinator_size){
                    while (combinator_grammar_list.size() <= inp_num) {
                        combinator_grammar_list.emplace_back(buildCombinatorGrammar(combinator_grammar_list.size()));
                    }
                    combinator_pool[inp_num] = enumeratePrograms(combinator_grammar_list[inp_num], std::max(combinator_size, int(combinator_pool[inp_num].size()) * 2));
                }
                std::vector<Program*> combinator_list;
                if (!collectCombinator(combinator_pool[inp_num], combinator_size, l_list, combinator_list, task)) {
                    is_valid = false;
                    break;
                }
                auto m = buildDefaultMergeInfo(task, combinator_list, l_list);
                merge_list.push_back(m);
            }
            if (is_valid) {
                lifting = l_list;
                merge_infos.clear();
                merge_infos.resize(l_list.size());
                for (int i = 0; i < l_list.size(); ++i) {
                    for (int j = 0; j < task_list.size(); ++j) {
                        merge_infos[i].push_back(merge_list[j][i]);
                    }
                }
                recorder::recorder.addRecord("#lifting", lifting.size());
                recorder::recorder.popStage();
                return;
            }
        }
        lift_size += step_size_1;
        combinator_size += step_size_2;
    }
}