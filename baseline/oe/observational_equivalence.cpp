//
// Created by pro on 2020/12/9.
//

#include "baseline/observational_equivalence.h"
#include "basic/enumerator.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

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
        if (result.size() == 1 && result[0]->toString() == "-1") {
            return {};
        }
        return result;
    }

    Grammar* buildDSL(Task* task) {
        auto* grammar = grammar::getDeepCoderDSL();
        grammar->addParam({TLIST});
        for (auto& extra_semantics: task->extra_semantics) {
            extra_semantics.insertExtraSemantics(grammar);
        }
        auto* start = new NonTerminal("start", TLIST);
        grammar->symbol_list.push_back(start);
        grammar->start_symbol = start;
        NonTerminal* int_symbol = nullptr;
        for (auto* symbol: grammar->symbol_list) {
            if (symbol->name == "int_expr") int_symbol = symbol;
        }
        auto* single_rule = new Rule(semantics::getSemanticsFromName("[]"), {int_symbol});
        auto* cons_rule = new Rule(semantics::getSemanticsFromName("cons"), {int_symbol, start});
        start->rule_list.push_back(single_rule);
        start->rule_list.push_back(cons_rule);
        return grammar;
    }

    DataList getInpList(const std::vector<std::pair<Example*, Example*>>& example_space) {
        std::set<std::string> S;
        DataList result;
        for (auto example: example_space) {
            for (auto* inp: {example.first, example.second}) {
                for (auto &d: *inp) {
                    if (d.getType() == TLIST) {
                        auto feature = d.toString();
                        if (S.find(feature) == S.end()) {
                            S.insert(feature);
                            result.emplace_back(d);
                        }
                    }
                }
            }
        }
        return result;
    }
}

class ObservationalVerifier: public Verifier {
    std::vector<std::pair<Example*, Example*> > counter_list;

    bool verify(Program* program, const std::pair<Example*, Example*>& example) {
        for (int i = 0; i < example.first->size(); ++i) {
            if ((*example.first)[i].getType() == TLIST) {
                auto x = (*example.first)[i], y = (*example.second)[i];
                if (program->run({x}) != program->run({y})) return true;
            }
        }
        return false;
    }
public:
    ObservationalVerifier(const std::vector<std::pair<Example*, Example*>>& _counter_list): counter_list(_counter_list) {}

    virtual bool verify(Program* program) {
        for (auto example: counter_list) {
            if (!verify(program, example)) {
                return false;
            }
        }
        return true;
    }
};

Program * ObservationalEquivalenceAuxSolver::synthesisFromExample(const std::vector<std::pair<Example *, Example *> > &example_list) {
    DataList inp_list = getInpList(example_list);
    auto* grammar = buildDSL(task);
    auto* v = new ObservationalVerifier(example_list);
    auto* enumerator = new OEEnumerator(grammar, v, inp_list);
    auto program = enumerator->synthesisProgram();
    delete grammar;
    delete v;
    delete enumerator;
    return program;
}

std::vector<Program *> ObservationalEquivalenceAuxSolver::synthesis() {
    int turn_num = 0;
    std::vector<std::pair<Example*, Example*>> example_list;
    while (true) {
        turn_num += 1;
        auto candidate_result = decomposeProgram(synthesisFromExample(example_list));
        if (config::KIsVerbose) {
            LOG(INFO) << turn_num;
            for (auto *p: candidate_result) {
                LOG(INFO) << "  " << p->toString();
            }
        }
        auto example = verify(candidate_result, false);
        if (example.first) {
            example_list.push_back(example);
        } else {
            return candidate_result;
        }
    }
}