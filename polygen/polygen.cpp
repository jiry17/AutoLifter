//
// Created by pro on 2021/5/5.
//

#include "polygen/polygen.h"
#include "basic/recorder.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include "glog/logging.h"

namespace {
    Program* mergeIte(std::vector<Program*>& term_list, std::vector<Program*>& condition_list) {
        int n = term_list.size();
        auto* program = term_list[n - 1];
        for (int i = n - 2; i >= 0; --i) {
            auto cond = condition_list[i]->toString();
            if (cond == "true") {
                program = term_list[i];
            } else if (cond == "false") {
                program = program;
            } else {
                program = new Program({condition_list[i], term_list[i], program},
                                      semantics::getSemanticsFromName("ite"));
            }
        }
        return program;
    }

    PointExample* verify(Program* program, const std::vector<PointExample*>& example_space) {
        for (auto* example: example_space) {
            if (program->run(example->first) != example->second) {
                return example;
            }
        }
        return nullptr;
    }

    std::vector<PointExample*> verifyTerms(const std::vector<Program*>& term_list,
            const std::vector<PointExample*>& example_space, int num = 1000) {
        std::vector<PointExample*> example_list;
        for (auto* example: example_space) {
            bool flag = false;
            for (auto* term: term_list) {
                if (term->run(example->first) == example->second) {
                    flag = true; break;
                }
            }
            if (!flag) {
                example_list.push_back(example);
                if (example_list.size() == num) return example_list;
            }
        }
        return example_list;
    }

    std::pair<std::vector<PointExample*>, std::vector<PointExample*> > verifyCondition(Program* condition,
            const std::vector<PointExample*>& positive_list, const std::vector<PointExample*>& negative_list,
            int num = 1) {
        std::vector<PointExample*> positive_result;
        for (auto* example: positive_list) {
            if (!condition->run(example->first).getBool()) {
                positive_result.push_back(example);
                if (positive_result.size() == num) break;
            }
        }
        std::vector<PointExample*> negative_result;
        for (auto* example: negative_list) {
            if (condition->run(example->first).getBool()) {
                negative_result.push_back(example);
                if (negative_result.size() == num) break;
            }
        }
        return std::make_pair(std::move(positive_result), std::move(negative_result));
    }

    std::vector<Program*> reorderTermList(const std::vector<Program*>& term_list,
            const std::vector<PointExample*>& example_list) {
        std::vector<std::pair<int, Program*>> count_list;
        for (auto* term: term_list) {
            int c = 0;
            for (auto* example: example_list) {
                if (term->run(example->first) == example->second) {
                    c += 1;
                }
            }
            count_list.emplace_back(c, term);
        }
        std::sort(count_list.begin(), count_list.end());
        std::vector<Program*> result;
        for (auto& info: count_list) {
            result.push_back(info.second);
        }
        return result;
    }
}

Program* PolyGen::phasedSynthesis(const std::vector<PointExample *> &full_example_list) {
    clearCache();

    /* todo: use a more clever way to deal with INF
    for (auto* example: full_example_list) {
        for (auto& d: example->first) {
            if (d.getInt() == config::KDefaultValue) d = 100;
        }
        if (example->second.getInt() == config::KDefaultValue) example->second = 100;
    }*/

    std::vector<Program*> term_list = {new Program({}, new ConstSemantics(0))};
    std::vector<PointExample*> counter_example_list;
    while (true) {
        if (config::KIsVerbose) {
            LOG(INFO) << "Finding Terms " << counter_example_list.size();
            for (auto *term: term_list) {
                LOG(INFO) << "    " << term->toString();
            }
            LOG(INFO) << "Start Verify";
        }
        auto example_list = verifyTerms(term_list, full_example_list);
        if (config::KIsVerbose) {
            LOG(INFO) << "End Verify " << example_list.size();
        }
        if (example_list.empty()) break;
        for (auto* example: example_list) {
            counter_example_list.push_back(example);
        }
        term_list = term_solver->getTerms(counter_example_list);
    }

    term_list = reorderTermList(term_list, full_example_list);

    LOG(INFO) << "Term Synthesis Finished";
    for (auto* term: term_list) {
        LOG(INFO) << "  " << term->toString();
    }

    std::vector<int> param_list; int param_num = full_example_list[0]->first.size();
    for (int i = 0; i < param_num; ++i) {
        param_list.push_back(i);
    }
    unifier->param_list = param_list;
    unifier->extra_list = term_list;

    std::vector<Program*> condition_list;
    std::vector<PointExample*> remain_list = full_example_list;
    for (int term_id = 0; term_id + 1 < term_list.size(); ++term_id) {
        LOG(INFO) << "synthesis condition for " << term_list[term_id]->toString();
        auto* term = term_list[term_id];
        std::vector<PointExample*> full_positive_list, full_negative_list, free_list;
        for (auto* example: remain_list) {
            if (term->run(example->first) != example->second)
                full_negative_list.push_back(example);
            else {
                bool flag = false;
                for (int next_id = term_id + 1; next_id < term_list.size(); ++next_id) {
                    if (term_list[next_id]->run(example->first) == example->second) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) full_positive_list.push_back(example);
                else free_list.push_back(example);
            }
        }

        std::vector<PointExample*> positive_list, negative_list;
        auto* condition = new Program({}, new ConstSemantics(new BoolValue(false)));
        while (1) {
            auto counter_list = verifyCondition(condition, full_positive_list, full_negative_list);
            if (counter_list.first.empty() && counter_list.second.empty()) break;
            /*if (config::KIsVerbose) {
                LOG(INFO) << "new counter " << counter_list.first.size() << " " << counter_list.second.size();
            }*/
            for (auto* example: counter_list.first) {
                positive_list.push_back(example);
            }
            for (auto* example: counter_list.second) {
                negative_list.push_back(example);
            }
            condition = unifier->getCondition(positive_list, negative_list);
        }
        LOG(INFO) << "Condition for " << term->toString() << ": " << condition->toString();
        condition_list.push_back(condition);

        remain_list = full_negative_list;
        for (auto* example: free_list) {
            if (!condition->run(example->first).getBool()) {
                remain_list.push_back(example);
            }
        }
    }
    return mergeIte(term_list, condition_list);
}

Program* PolyGen::synthesis(const std::vector<PointExample*>& full_example_space) {
    if (!full_example_space.empty()) {
        bool is_constant = true;
        for (auto* example: full_example_space) {
            if (full_example_space[0]->second != example->second) {
                is_constant = false; break;
            }
        }
        if (is_constant) {
            return new Program({}, new ConstSemantics(full_example_space[0]->second.getInt()));
        }
    }
    return phasedSynthesis(full_example_space);
}