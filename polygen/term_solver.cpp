//
// Created by pro on 2021/2/23.
//

#include "polygen/term_solver.h"
#include "basic/semantics_factory.h"
#include "basic/bitset.h"
#include "glog/logging.h"
#include "basic/recorder.h"
#include "basic/util.h"
#include "basic/config.h"
#include "basic/enumerator.h"

#include <set>
#include <random>

namespace {
    int term_int_max = 0;
    int max_example_num = 0;
    int max_term_num = 0;

    int getConstBound() {
        int num = term_int_max;
        for (auto& int_const: global::int_consts) {
            num = std::max(num, int_const);
        }
    }

    double convertToInt(double x) {
        int w = int(x);
        while (w < x - 0.5) ++w;
        while (w > x + 0.5) --w;
        return w;
    }

    int specialTreatmentForError(const std::vector<PointExample*> &example_list, std::vector<int>& result) {
        int n = example_list[0]->first.size();
        result.resize(n + 1);
        for (auto& v: result) v = 0;
        for (int i = 0; i < n; ++i) {
            bool is_valid = true;
            for (auto* example: example_list) {
                if (example->first[i] != example->second) {
                    is_valid = false; break;
                }
            }
            if (is_valid) {
                result[i] = 1; return 1;
            }
        }
        if (!example_list[0]->second.isError()) return 0;
        int cons = example_list[0]->second.getInt();
        for (auto* example: example_list) {
            if (example->second.getInt() != cons) {
                return 0;
            }
        }
        result[n] = cons;
        return 1;
    }

    std::map<GRB_DoubleParam, double> local_gurobi_param;

    void relaxEpsForGuorobi(GRBEnv& env) {
        std::vector<GRB_DoubleParam> param_list = {GRB_DoubleParam_IntFeasTol,
                                                   GRB_DoubleParam_FeasibilityTol};
        for (auto& param: param_list) {
            if (local_gurobi_param.count(param) == 0) {
                local_gurobi_param[param] = 1e-8;
            } else {
                local_gurobi_param[param] *= -10;
            }
            env.set(param, local_gurobi_param[param]);
        }
    }

    int getOptimalAssignment(GRBEnv& env, const std::vector<PointExample*> &example_list, std::vector<int>& result) {
        int n = example_list[0]->first.size();

        /*for (auto* example: example_list) {
            LOG(INFO) << util::dataList2String(example->first) << " " << example->second.toString();
        }*/
        // Special Treatment for INF (config::KDefaultValue)
        bool is_contain_error_example = false;
        for (auto* example: example_list) {
            if (example->second.isError()) {
                is_contain_error_example = true;
            }
        }
        if (is_contain_error_example) return specialTreatmentForError(example_list, result);

        std::vector<bool> is_error_pos(n);
        for (int i = 0; i < n; ++i) {
            is_error_pos[i] = false;
            for (auto* example: example_list) {
                if (example->first[i].isError()) {
                    is_error_pos[i] = true;
                    break;
                }
            }
        }

        GRBModel model = GRBModel(env);
        model.set(GRB_IntParam_OutputFlag, 0);
        std::vector<GRBVar> var_list;
        std::vector<GRBVar> bound_list;
        for (int i = 0; i <= n; ++i) {
            std::string name_var = "var" + std::to_string(i);
            std::string name_bound = "bound" + std::to_string(i);
            int bound = i < n ? config::KTermIntMax : getConstBound();
            var_list.push_back(model.addVar(-bound, bound, 0.0, GRB_INTEGER, name_var));
            if (i == n) {
                bound_list.push_back(model.addVar(0, 1, 0.0, GRB_INTEGER, name_bound));
                model.addConstr(var_list[i] <= bound * bound_list[i], "rbound" + std::to_string(i));
                model.addConstr(var_list[i] >= -bound * bound_list[i], "lbound" + std::to_string(i));
            } else {
                bound_list.push_back(model.addVar(0, bound, 0.0, GRB_INTEGER, name_bound));
                model.addConstr(var_list[i] <= bound_list[i], "rbound" + std::to_string(i));
                model.addConstr(var_list[i] >= -bound_list[i], "lbound" + std::to_string(i));
            }
        }

        int id = 0;
        for (auto& example: example_list) {
            GRBLinExpr expr = var_list[n];
            for (int i = 0; i < n; ++i) {
                if (is_error_pos[i]) continue;
                expr += example->first[i].getInt() * var_list[i];
            }
            model.addConstr(expr == example->second.getInt(), "cons" + std::to_string(id++));
        }
        GRBLinExpr target = 0;
        for (auto bound_var: bound_list) {
            target += bound_var;
        }
        model.setObjective(target, GRB_MINIMIZE);
        model.optimize();
        int status = model.get(GRB_IntAttr_Status);
        if (status == GRB_INFEASIBLE) {
            result.clear();
            return 0;
        }
        // std::cout << "find solution" << std::endl;
        result.clear();
        for (auto var: var_list) {
            result.push_back(convertToInt(var.get(GRB_DoubleAttr_X)));
        }
        // for (auto now: result) std::cout << now << " "; std::cout << std::endl;
#ifdef DEBUG
        for (auto& example: example_list) {
            int partial_oup = result[n];
            for (int i = 0; i < n; ++i) {
                partial_oup += result[i] * example->first[i].getInt();
            }
            if (partial_oup != example->second.getInt()) {
                std::cout << partial_oup << " " << example->second.getInt() << std::endl;
                std::cout << util::dataList2String(example->first) << std::endl;
                for (int i = 0; i < n; ++i) {
                    std::cout << is_error_pos[i] << " ";
                }
                std::cout << std::endl;
                for (int i = 0; i <= n; ++i) std::cout << result[i] << " "; std::cout << std::endl;
                relaxEpsForGuorobi(env);
            }
        }
#endif
        return 1;
    }


    std::mt19937 rng;
    std::uniform_real_distribution<double> distribution(0, 1);

    int program_size_limit;

    Program* buildAssignment(const std::vector<int>& assignment) {
        std::vector<Program*> term_list;
        auto* times = semantics::getSemanticsFromName("*");
        auto* plus = semantics::getSemanticsFromName("+");
        int n = assignment.size() - 1;
        for (int i = 0; i < n; ++i) {
            if (assignment[i]) {
                auto* var = new Program({}, new ParamSemantics(i, TINT));
                if (assignment[i] != 1) {
                    auto *k = new Program({}, new ConstSemantics(assignment[i]));
                    term_list.push_back(new Program({var, k}, times));
                } else {
                    term_list.push_back(var);
                }
            }
        }
        if (assignment[n] || term_list.empty()) {
            term_list.push_back(new Program({}, new ConstSemantics(assignment[n])));
        }
        auto* result = term_list[0];
        for (int i = 1; i < term_list.size(); ++i) {
            result = new Program({result, term_list[i]}, plus);
        }
        return result;
    }

    bool is_print = false;

    bool verifyAssignment(const std::vector<int>& assignment, PointExample* example) {
        int oup = assignment[example->first.size()];
        for (int i = 0; i < example->first.size(); ++i) {
            oup += assignment[i] * example->first[i].getInt();
        }
        return oup == example->second.getInt();
    }

    struct AssignmentInfo {
        std::vector<int> assignment;
        Bitset P;
        int cost;
        AssignmentInfo(const std::vector<int>& _assignment): assignment(_assignment), cost(0) {
            for (int& v: assignment) cost += !!v;
        }
        void update(const std::vector<PointExample*>& example_list) {
            for (int i = P.size(); i < example_list.size(); ++i) {
                P.append(verifyAssignment(assignment, example_list[i]));
            }
        }
    };

    struct AssignmentCmp {
        Bitset all;
        AssignmentCmp(const Bitset& _all): all(_all) {}
        int operator () (AssignmentInfo* info_1, AssignmentInfo* info_2) {
            return (info_1->P & all).count() > (info_2->P & all).count();
        }
    };

    std::map<std::vector<int>, AssignmentInfo*> info_map, solved_sample;

    AssignmentInfo* buildAssignmentInfo(const std::vector<int>& assignment, const std::vector<PointExample*>& example_list) {
        if (info_map.count(assignment)) {
            auto* result = info_map[assignment];
            result->update(example_list);
            return result;
        }
        auto* info = new AssignmentInfo(assignment);
        info->update(example_list);
        info_map[assignment] = info;
        return info;
    }

    struct SampleInfo {
        int status;
        std::vector<int> example_list;
        AssignmentInfo* result;
        SampleInfo(const std::vector<int>& _example_list): example_list(_example_list), status(0), result(nullptr) {
            std::sort(example_list.begin(), example_list.end());
        }
    };

    int K_limit;

    struct TermPlan {
        int N;
        std::vector<AssignmentInfo*> term_list;
        std::vector<int> rem_example;
        Bitset info;
        std::vector<SampleInfo*> sample_list;
        TermPlan(int _N, const std::vector<AssignmentInfo*>& _term_list, const Bitset& _info, const std::vector<SampleInfo*>& _sample_list):
                N(_N), term_list(_term_list), info(_info), sample_list(_sample_list) {}
        TermPlan(int _N, const std::vector<AssignmentInfo*>& _term_list): N(_N), term_list(_term_list) {}

        bool checkCover(SampleInfo* sample) {
            for (int& id: sample->example_list) if (!info[id]) return false;
            return true;
        }

        void print() {
            std::cout << "term plan with n = " << N << " rem_num = " << rem_example.size() << std::endl;
            for (auto* info: term_list) {
                std::cout << buildAssignment(info->assignment)->toString() << std::endl;
            }
            /*for (auto* sample: sample_list) {
                std::cout << "sample " << sample->status << " " << (sample->result ? buildAssignment(
                        sample->result->assignment)->toString() : "Fail") << std::endl;
                for (auto& id: sample->example_list) std::cout << id << " "; std::cout << std::endl;
            }*/
        }

        SampleInfo* generateSample(int type, int mid) {
            std::vector<int> id_list(N);
            if (type == 0) {
                for (int i = 0; i < N; ++i) id_list[i] = rem_example[std::rand() % mid];
            } else {
                while (1) {
                    bool is_valid = false;
                    for (int i = 0; i < N; ++i) {
                        int now = rem_example[std::rand() % rem_example.size()];
                        if (now >= mid) is_valid = true;
                        id_list[i] = now;
                    }
                    if (is_valid) break;
                }
            }
            // TODO: reduce duplicate sample
            return new SampleInfo(id_list);
        }
    };

    struct TermPlanCmp {
        bool operator () (TermPlan* plan_1, TermPlan* plan_2) const {
            if (plan_1->N < plan_2->N) return true;
            if (plan_1->N > plan_2->N) return false;
            if (plan_1->term_list.size() < plan_2->term_list.size()) return true;
            if (plan_1->term_list.size() > plan_2->term_list.size()) return false;
            for (int i = 0; i < plan_1->term_list.size(); ++i) {
                if (plan_1->term_list[i]->assignment == plan_2->term_list[i]->assignment) continue;
                return plan_1->term_list[i]->assignment < plan_2->term_list[i]->assignment;
            }
            return false;
        }
    };

    std::set<TermPlan*, TermPlanCmp> plan_set;

    std::vector<PointExample*> idToExample(const std::vector<int>& id_list, const std::vector<PointExample*>& example_space) {
        std::vector<PointExample*> result;
        for (auto& id: id_list) result.push_back(example_space[id]);
        return result;
    }


    void performSample(GRBEnv& env, SampleInfo* sample, const std::vector<PointExample*>& example_space) {
        if (sample->status) return;
        if (solved_sample.count(sample->example_list)) {
            sample->result = solved_sample[sample->example_list];
            if (sample->result) sample->result->update(example_space);
            return;
        }
        sample->status = 1;
        auto example_list = idToExample(sample->example_list, example_space);
        std::vector<int> assignment;
        int status = -1;
        status = getOptimalAssignment(env, example_list, assignment);
        if (status == 0) {
            solved_sample[sample->example_list] = nullptr;
            return;
        }
        sample->result = buildAssignmentInfo(assignment, example_space);
        solved_sample[sample->example_list] = sample->result;
    }

    std::vector<AssignmentInfo*> insertTermList(TermPlan* plan, AssignmentInfo* info) {
        auto result = plan->term_list;
        int pos = 0;
        while (pos < plan->term_list.size() && plan->term_list[pos]->assignment > info->assignment) {
            ++pos;
        }
        result.push_back(nullptr);
        for (int i = result.size() - 1; i > pos; --i) result[i] = result[i - 1];
        result[pos] = info;
        return result;
    }

    int calculateRandomTime(int branch_num, int N) {
        int ti = config::KRandomC;
        for (int i = 1; i <= N; ++i) ti *= branch_num;
        return ti;
    }

    void extendStart(TermPlan* plan, const std::vector<PointExample*>& example_space) {
        int pre_size = plan->info.size();
        int n = plan->N;
        for (int i = pre_size; i < example_space.size(); ++i) {
            plan->info.append(true);
            plan->rem_example.push_back(i);
        }
        for (auto* sample: plan->sample_list) {
            if (sample->result) sample->result->update(example_space);
        }
        std::vector<SampleInfo*> new_sample;
        int max_rem = K_limit - plan->term_list.size();
        int max_sample_num = calculateRandomTime(max_rem, n);
        double p = std::pow(1.0 * pre_size / plan->rem_example.size(), n);
        int pos = 0;
        while (pos < plan->sample_list.size() && new_sample.size() < max_sample_num) {
            if (distribution(rng) <= p) {
                new_sample.push_back(plan->sample_list[pos++]);
            } else {
                new_sample.push_back(plan->generateSample(1, pre_size));
            }
        }
        plan->sample_list = new_sample;
    }

    void extendPlan(TermPlan* plan, AssignmentInfo* info, TermPlan* pre, const std::vector<PointExample*>& example_space) {
        int pre_size = plan->info.size();
        int pre_num = plan->rem_example.size();
        for (int i = pre_size; i < example_space.size(); ++i) {
            if (!info->P[i] && pre->info[i]) {
                plan->info.append(true);
                plan->rem_example.push_back(i);
            } else {
                plan->info.append(false);
            }
        }
        for (auto* sample: plan->sample_list) {
            if (sample->result) sample->result->update(example_space);
        }
        std::vector<SampleInfo*> reusable_result;
        std::vector<SampleInfo*> new_sample;
        int n = plan->N;
        for (auto* sample: pre->sample_list) {
            if (plan->checkCover(sample) && sample->example_list[n - 1] >= pre_size) {
                reusable_result.push_back(sample);
            }
        }
        double p = std::pow(1.0 * pre_num / plan->rem_example.size(), n);
        int l_pos = 0, r_pos = 0;
        int max_rem = K_limit - plan->term_list.size();
        int max_sample_num = calculateRandomTime(max_rem, n);
        while ((l_pos < plan->sample_list.size() || r_pos < reusable_result.size()) && new_sample.size() < max_sample_num) {
            if (distribution(rng) <= p) {
                if (l_pos < plan->sample_list.size()) {
                    new_sample.push_back(plan->sample_list[l_pos++]);
                } else {
                    new_sample.push_back(plan->generateSample(0, pre_num));
                }
            } else {
                if (r_pos < reusable_result.size()) {
                    new_sample.push_back(reusable_result[r_pos++]);
                } else {
                    new_sample.push_back(plan->generateSample(1, pre_num));
                }
            }
        }
        plan->sample_list = new_sample;
    }

    TermPlan* buildTermPlan(TermPlan* plan, AssignmentInfo* info, const std::vector<PointExample*>& example_space) {
        auto term_list = insertTermList(plan, info);
        auto* dummy_plan = new TermPlan(plan->N, term_list);
        auto it = plan_set.find(dummy_plan);
        if (it != plan_set.end()) {
            auto* result = *it; delete dummy_plan;
            extendPlan(result, info, plan, example_space);
            return result;
        } else {
            auto* result = dummy_plan;
            plan_set.insert(result);
            extendPlan(result, info, plan, example_space);
            return result;
        }
    }

    std::vector<AssignmentInfo*> getNextAssignment(GRBEnv& env, TermPlan* plan, const std::vector<PointExample*>& example_space, int N, int rem_branch) {
        int ti = calculateRandomTime(rem_branch, N);
        int limit = (int(plan->rem_example.size()) - 1) / rem_branch + 1;
        while (plan->sample_list.size() < ti) {
            plan->sample_list.push_back(plan->generateSample(0, plan->rem_example.size()));
        }
        std::set<AssignmentInfo*> info_set;
        std::set<AssignmentInfo*> fail_set;
        std::vector<AssignmentInfo*> result;
        for (int i = 0; i < ti; ++i) {
            auto* sample = plan->sample_list[i];
            performSample(env, sample, example_space);
            auto* info = sample->result;
            if (!info) continue;
            if (info_set.find(info) == info_set.end() && (plan->info & info->P).count() >= limit && info->cost <= program_size_limit) {
                info_set.insert(info);
                result.push_back(info);
            } else {
                fail_set.insert(info);
            }
        }
        if (is_print) {
            std::cout << "success" << std::endl;
            for (auto res: info_set) {
                std::cout << buildAssignment(res->assignment)->toString() << " ";
            }
            std::cout << std::endl;
            if (info_set.empty()) {
                std::cout << "fail" << std::endl;
                for (auto res: fail_set) {
                    std::cout << buildAssignment(res->assignment)->toString() << " ";
                }
                std::cout << std::endl;
            }
        }
        return result;
    }

    std::set<TermPlan*> visited_plan;

    bool search(GRBEnv& env, TermPlan* plan, const std::vector<PointExample*>& example_space, std::vector<Program*>& result, int N, int rem_branch) {
        if (rem_branch == 0 || plan->rem_example.empty()) {
            assert(plan->rem_example.empty());
            result.clear();
            for (auto* assignment: plan->term_list) {
                result.push_back(buildAssignment(assignment->assignment));
            }
            return true;
        }
        if (is_print) plan->print();
        auto next_assignment = getNextAssignment(env, plan, example_space, N, rem_branch);
        std::sort(next_assignment.begin(), next_assignment.end(), AssignmentCmp(plan->info));
        for (auto* info: next_assignment) {
            auto* next_plan = buildTermPlan(plan, info, example_space);
            if (visited_plan.find(next_plan) == visited_plan.end()) {
                visited_plan.insert(next_plan);
                if (search(env, next_plan, example_space, result, N, rem_branch - 1)) return true;
            }
        }
        return false;
    }

    std::vector<Program*> removeDuplicatedTerm(const std::vector<Program*>& term_list, const std::vector<PointExample*>& example_list) {
        std::set<std::string> S;
        std::vector<Program*> result;
        // LOG(INFO) << "remove";
        // LOG(INFO) << util::dataList2String(example_list[0]->first) << " " << example_list[0]->second.toString() << std::endl;
        for (auto* term: term_list) {
            // LOG(INFO) << term->toString();
            DataList oup_list;
            for (auto* example: example_list) {
                oup_list.push_back(term->run(example->first));
            }
            auto feature = util::dataList2String(oup_list);
            if (S.find(feature) == S.end()) {
                result.push_back(term);
                S.insert(feature);
            }
        }
        return result;
    }

    std::vector<Program*> getAtomList(int n, int size, const std::vector<ExternalExtraSemantics>& semantics_list) {
        auto* param = new NonTerminal("param", TINT);
        for (int i = 0; i < n; ++i) {
            param->rule_list.push_back(new Rule(new ParamSemantics(i, TINT), {}));
        }
        auto* start = new NonTerminal("int_expr", TINT);
        start->rule_list.push_back(new Rule(semantics::getSemanticsFromName("int"), {param}));
        auto* grammar = new Grammar(start, {start, param});
        for (auto& extra_semantics: semantics_list) {
            extra_semantics.insertExternalExtraSemantics(grammar);
        }
        auto* enumerator = new Enumerator(grammar, size);
        auto result = enumerator->enumerateProgram(config::KDefaultValue);
        return result;
    }

    Program* rewriteTermWithAtoms(Program* term, const std::vector<Program*>& atom_list) {
        auto* param_semantics = dynamic_cast<ParamSemantics*>(term->semantics);
        if (param_semantics) {
            return atom_list[param_semantics->getId()]->copy();
        }
        std::vector<Program*> sub_list;
        for (auto* sub_program: term->sub_list) {
            sub_list.push_back(rewriteTermWithAtoms(sub_program, atom_list));
        }
        return new Program(sub_list, term->semantics);
    }

    bool isPlus(Program* p) {
        return p->semantics->name == "+";
    }


    std::vector<Program*> filterOutInvalidTerm(const std::vector<Program*>& term_list, const std::vector<PointExample*>& example_list) {
        std::vector<Program*> result;
        for (auto* term: term_list) {
            if (isPlus(term)) continue;
            bool is_valid = true;
            for (auto* example: example_list) {
                auto oup = term->run(example->first);
                if (!oup.isError() && oup.getInt() >= config::KExampleIntMax) {
                    is_valid = false; break;
                }
            }
            if (is_valid) {
                result.push_back(term);
            }
        }
        return result;
    }
}

std::vector<Program *> TermSolver:: getTerms(const std::vector<PointExample *> &example_space, int N, int K) {
    visited_plan.clear();
    std::vector<int> full_example;

    auto* start = new TermPlan(N, {});
    auto it = plan_set.find(start);
    if (it == plan_set.end()) {
        plan_set.insert(start);
    } else {
        delete start; start = *it;
    }
    extendStart(start, example_space);
    std::vector<Program*> result;
    search(env, start, example_space, result, N, K);
    return result;
}

std::vector<PointExample *> TermSolver::buildExtendedExamples(const std::vector<Program*>& atom_list, const std::vector<PointExample *> &example_list) {
    std::vector<PointExample*> complete_example_list;
    for (auto* example: example_list) {
        DataList inp_list;
        for (auto* term: atom_list) inp_list.emplace_back(term->run(example->first));
        for (auto& inp_value: inp_list) {
            if (!inp_value.isError() && std::abs(inp_value.getInt()) >= config::KExampleIntMax) {
                /*std::cout << "fail" << std::endl;
                std::cout << util::dataList2String(example->first) << " " << example->second.toString() << std::endl;
                std::cout << util::dataList2String(inp_list);*/
                return {};
            }
        }
        complete_example_list.push_back(new PointExample(inp_list, example->second));
    }
    return complete_example_list;
}

std::vector<Program *> TermSolver::getTerms(const std::vector<PointExample *> &example_list, int N) {
    for (int i = 1; i <= max_term_num; ++i) {
        LOG(INFO) << "Branch num " << i;
        program_size_limit = 2 * N * i;
        auto result = getTerms(example_list, N, i);
        if (!result.empty()) return result;
    }
    return {};
}

std::vector<Program *> TermSolver::getTerms(const std::vector<PointExample *> &example_list) {

    if (example_list.empty()) return {new Program({}, new ConstSemantics(0))};
    int n = example_list[0]->first.size();
    local_gurobi_param.clear();

    max_term_num = config::KMaxBranchNum;
    max_example_num = config::KMaxTermNum;
    term_int_max = config::KTermIntMax;

    std::vector<std::vector<Program*>> atom_cache;
    std::vector<std::vector<PointExample*>> atom_example_cache;
    std::vector<int> atom_progress;
    std::set<std::string> atom_feature_map;
    int atom_size = 1;
    auto extend_cache = [&]() {
        for (int _ = 0; _ < 3; _++) {
            ++atom_size;
            std::vector<Program*> atom_list = getAtomList(n, atom_size, task->extra_sygus_semantics);

            atom_list = removeDuplicatedTerm(atom_list, example_list);
            atom_list = filterOutInvalidTerm(atom_list, example_list);
            std::string atom_feature;
            for (auto* atom: atom_list) {
                atom_feature += atom->toString() + "@";
            }
            if (atom_feature_map.find(atom_feature) != atom_feature_map.end()) continue;
            auto extended_example = buildExtendedExamples(atom_list, example_list);
            if (extended_example.empty()) continue;
            atom_feature_map.insert(atom_feature);
            atom_cache.push_back(atom_list);
            atom_progress.push_back(0);
            atom_example_cache.emplace_back(std::move(extended_example));
            return true;
        }
        return false;
    };

    std::set<std::vector<int>> calculated_set;

    while (1) {
        if (extend_cache()) {
            for (auto* a: atom_cache[atom_cache.size() - 1]) {
                bool is_valid = true;
                for (auto* e: example_list) {
                    if (a->run(e->first) != e->second) {
                        is_valid = false;
                        break;
                    }
                }
                if (is_valid) return {a};
            }
        }
        for (int si = 0; si < atom_cache.size(); ++si) {
            if (!atom_cache.empty()) clearCache();
            atom_progress[si] += 2;
            int N_limit = std::min(config::KExampleIntMax, atom_progress[si] / 2);
            int K_limit = std::min(config::KMaxBranchNum, atom_progress[si]);
            for (int N = 1; N <= N_limit; ++N) {
                for (int K = 1; K <= K_limit; ++K) {
                    if (calculated_set.find({si, N, K}) != calculated_set.end()) continue;
                    program_size_limit = 2 * N * K;
                    calculated_set.insert({si, N, K});
                    auto result = getTerms(atom_example_cache[si], N, K);
                    if (!result.empty()) {
                        for (auto &atom_example_list: atom_example_cache) {
                            for (auto *example: atom_example_list) {
                                delete example;
                            }
                        }
                        for (int i = 0; i < result.size(); ++i) {
                            result[i] = rewriteTermWithAtoms(result[i], atom_cache[si]);
                        }
                        return result;
                    }
                }
            }
        }
    }
/*
    for (int atom_size = 1;; ++atom_size) {
        std::vector<Program*> atom_list = getAtomList(n, atom_size, task->extra_sygus_semantics);
        atom_list = removeDuplicatedTerm(atom_list, example_list);

        std::string atom_feature;
        for (auto* atom: atom_list) {
            atom_feature += atom->toString() + ";";
        }
        if (S1.find(atom_feature) != S1.end()) continue;
        S1.insert(atom_feature);

        std::vector<PointExample*> extended_example_list = buildExtendedExamples(atom_list, example_list);

        for (int N = 1; N <= max_example_num; ++N) {
            LOG(INFO) << "Current Stage " << atom_size << " " << N;
            auto result = getTerms(extended_example_list, N);
            if (!result.empty()) {
                for (auto *example: extended_example_list) {
                    delete example;
                }
                for (int i = 0; i < result.size(); ++i) {
                    result[i] = rewriteTermWithAtoms(result[i], atom_list);
                }
                return result;
            }
        }

        for (auto* example: extended_example_list) {
            delete example;
        }
        clearCache();
        term_int_max *= 2;
        ++max_term_num;
        ++max_example_num;
    }*/
}

void TermSolver::clearCache() {
    info_map.clear();
    solved_sample.clear();
    plan_set.clear();
    visited_plan.clear();
}