//
// Created by pro on 2021/1/20.
//

#include "autolifter/observational_covering.h"
#include "basic/recorder.h"
#include "basic/util.h"

#include "glog/logging.h"

namespace {
    std::unordered_map<std::string, int> data_map;

    bool isBetter(const std::vector<Program*>& x, const std::vector<Program*>& y) {
        int total_size = 0;
        for (auto* p: x) total_size += p->size();
        for (auto* p: y) total_size -= p->size();
        return total_size < 0;
    }

    int getExampleLimit(const std::vector<Program*>& x) {
        if (x.empty()) return 0;
        int ans = -1;
        for (auto* p: x) {
            ans += 1 + p->size();
        }
        return ans * 2;
    }

    int vis_f = 0;
}

ObservationalCoveringAuxSolver* current = nullptr;

void ObservationalCoveringAuxSolver::getMoreComponents() {
    int num = program_space.size();
    if (num == 0) num = 10000; else num <<= 1;
    //grammar->print();
    //int kk; std::cin >> kk;
    auto* enumerator = new Enumerator(grammar);
    auto new_program_space = enumerator->enumerateProgram(num);
#ifdef DEBUG
    for (int i = 0; i < program_space.size(); ++i) {
        assert(program_space[i]->toString() == new_program_space[i]->toString());
    }
#endif
    program_space = new_program_space;
}

HASH ObservationalCoveringAuxSolver::getComponentHash(int k) {
    while (hash_pool.size() <= k) {
        hash_pool.push_back((HASH(rand()) << 32u) + rand());
    }
    return hash_pool[k];
}

void ObservationalCoveringAuxSolver::insertInput(Example* input) {
    auto feature = util::dataList2String(*input);
    inp_pool[feature] = input;
}


void ObservationalCoveringAuxSolver::addCounterExample(std::pair<Example *, Example *> counter_example) {
    for (int i = 0; i < program_info.size(); ++i) {
        program_info[i].append(task->evaluate(program_space[i], counter_example));
    }
    example_list.push_back(counter_example);
    for (auto& infos: info_list) {
        for (auto* info: infos) delete info;
    }
    insertInput(counter_example.first);
    insertInput(counter_example.second);
    info_list.clear();
    next_component_id = 0;
    maximal_list.clear();
    global.clear();
    maximal_set.clear();
    last.clear();
}

void ObservationalCoveringAuxSolver::initNewProgram() {
    if (program_info.size() == program_space.size()) {
        getMoreComponents();
    }
    int n = program_info.size();
    auto* program = program_space[n];
    program_info.emplace_back();
    for (auto& example: example_list) {
        bool now = task->evaluate(program, example);
        program_info[n].append(now);
    }
}

ObservationalCoveringAuxSolver::ObservationalCoveringAuxSolver(Task *_task):
    Solver(_task), grammar(grammar::getDeepCoderDSL()) {
    grammar->addParam({TLIST});
    for (auto& extra_semantics: task->extra_semantics) {
        // LOG(INFO) << "Insert extra semantics";
        extra_semantics.insertExtraSemantics(grammar);
    }
    current = this;
}

bool UncoveredInfoList::isAdd(EnumerateInfo *x) {
    for (int i = num - 1; i >= 0; --i) {
        if (info_list[i]->info.checkCover(x->info)) {
            /*std::cout << "fail " << std::endl;
            for (auto ind: info_list[i]->ind_list) std::cout << current->program_space[ind]->toString() << std::endl;
            std::cout << x->info.toString() << " " << info_list[i]->info.toString() << std::endl;*/
            return false;
        }
    }
    return true;
}

int UncoveredInfoList::add(EnumerateInfo *x) {
    int now = 0;
    bool flag = false;
    bool is_insert = true;
    for (int i = 0; i < num; ++i) {
        vis_f++;
        auto* pre = info_list[i];
        if (pre->ind_list[pre->ind_list.size() - 1] < x->ind_list[0] && (pre->info|x->info).count() == x->info.size()) {
            flag = true;
        }
        auto overlap_num = (pre->info & x->info).count();
        if (overlap_num == x->info.count()) return flag;
        if (overlap_num != pre->info.count()) {
            info_list[now++] = pre;
        }
    }
    if (now == info_list.size()) {
        info_list.push_back(x);
    } else {
        info_list[now] = x;
    }
    num = now + 1;
    return flag;
}

bool ObservationalCoveringAuxSolver::isAddExpression(int k, EnumerateInfo* x) {
    // if (k > 0 && evaluateOverfitRate(x) > config::KOverfitLimit) return false;
    if (!maximal_list[k].isAdd(x)) return false;
    // std::cout << "go first " << std::endl;
    if (k > 0) {
        for (auto ind: x->ind_list) {
            auto now_hash = x->h ^ getComponentHash(ind);
            if (maximal_set.find(now_hash) == maximal_set.end()) return false;
        }
    }
    return true;
}

std::pair<EnumerateInfo*, EnumerateInfo*> ObservationalCoveringAuxSolver::recoverResult(EnumerateInfo *info) {
    for (auto& infos: info_list) {
        for (auto* x: infos) {
            if ((x->info | info->info).count() == info->info.size()) {
                return {x, info};
            }
        }
    }
    assert(0);
}

std::pair<EnumerateInfo*, EnumerateInfo*> ObservationalCoveringAuxSolver::addExpression(int k, EnumerateInfo *info) {
    /*std::cout << "try add " << std::endl;
    for (auto ind: info->ind_list) std::cout << current->program_space[ind]->toString() << std::endl;*/
    if (!isAddExpression(k, info)) {
        delete info;
        return {};
    }
    info_list[k].push_back(info);
    maximal_list[k].add(info);
    int status = global.add(info);
    maximal_set.insert(info->h);
    if (status) return recoverResult(info);
    return {nullptr, nullptr};
}

std::pair<EnumerateInfo*, EnumerateInfo*> ObservationalCoveringAuxSolver::getNextComposition(int k) {
    int l_id = 0, r_id = 0;
    while (info_list.size() <= k) {
        info_list.emplace_back();
    }
    while (last.size() <= k) {
        last.emplace_back(-1, 0);
    }
    while (maximal_list.size() <= k) {
        maximal_list.emplace_back();
    }
    while (k) {
        if (info_list[k].empty()) l_id = 0, r_id = 0;
        else {
            l_id = last[k].first + 1;
            r_id = last[k].second;
        }
        while (r_id < info_list[k - 1].size() && info_list[k - 1][r_id]->l_id <= l_id) {
            l_id = 0; ++r_id;
        }
        if (r_id >= info_list[k - 1].size()) --k; else break;
    }
    last[k] = std::make_pair(l_id, r_id);
    if (k == 0) {
        vis_f++;
        if (next_component_id == program_info.size()) {
            initNewProgram();
        }
        int n = info_list[0].size();
        auto* new_component = new EnumerateInfo(n, n, getComponentHash(next_component_id),
                {next_component_id}, program_info[next_component_id]);
        if (program_info[next_component_id].count() == example_list.size()) {
            return {new_component, nullptr};
        }
        next_component_id += 1;
        return addExpression(k, new_component);
    } else {
        std::vector<int> ind_list = {info_list[0][l_id]->ind_list[0]};
        for (auto ind: info_list[k - 1][r_id]->ind_list) {
            ind_list.push_back(ind);
        }
        Bitset info = info_list[0][l_id]->info | info_list[k - 1][r_id]->info;
        auto* new_component = new EnumerateInfo(l_id, r_id, info_list[0][l_id]->h ^ info_list[k - 1][r_id]->h,
                ind_list, info);
        return addExpression(k, new_component);
    }
}

std::vector<Program *> ObservationalCoveringAuxSolver::getProgramList(EnumerateInfo *info) {
    if (!info) return {};
    std::vector<Program*> program_list;
    for (auto ind: info->ind_list) {
        program_list.push_back(program_space[ind]);
    }
    return program_list;
}

std::vector<Program *> ObservationalCoveringAuxSolver::synthesisFromExample() {
    if (example_list.size() == 0) return {};
    int extra_turn_num = -1;
    std::vector<Program *> best_result;
    for (int turn_id = 1;; ++turn_id) {
        int k = (turn_id - 1) % 2;
        if (!best_result.empty()) {
            extra_turn_num--;
            k = 0;
        }
        auto info = getNextComposition(k);
        if (info.first) {
            std::vector<Program *> result;
            for (auto p: getProgramList(info.first)) {
                result.push_back(p);
            }
            if (info.second) {
                for (auto p: getProgramList(info.second)) {
                    result.push_back(p);
                }
            }
            if (best_result.empty() || isBetter(result, best_result)) {
                best_result = result;
                extra_turn_num = config::KExtraTurnNum;
            }
        }
        if (extra_turn_num == 0 || best_result.size() == 1) return best_result;
    }
}

std::pair<Example *, Example *> ObservationalCoveringAuxSolver::verifyOccam(const std::vector<Program *> &program_list) {
    data_map.clear();
    std::vector<Program*> complete_program_list = program_list;
    complete_program_list.push_back(task->target);

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
    int target_num = getExampleLimit(program_list) * task->example_space->c.init_size;

    acquireMoreExample(target_num);
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

void ObservationalCoveringAuxSolver::recordRes(const ProgramList &res, int turn_num, int f_num, int syntax_num) {
    int depth = 0, size = -1;
    std::vector<int> depth_list;
    for (auto* p: res) {
        depth_list.push_back(p->depth());
        size += 1 + p->size();
    }
    std::sort(depth_list.begin(), depth_list.end());
    for (int i = 0; i < depth_list.size(); ++i) {
        depth = std::max(depth, i + depth_list[i] + 1);
    }
}


std::vector<Program*> ObservationalCoveringAuxSolver::synthesis() {
    int turn_num = 0, tot = 0;
    while (true) {
        turn_num += 1; tot = 0; vis_f = 0;
        auto candidate_result = synthesisFromExample();
        if (config::KIsVerbose) {
            LOG(INFO) << "Turn " << turn_num << std::endl;
            for (int i = 0; i < last.size(); ++i) {
                LOG(INFO) << "(" << i << "): " << info_list[i].size() << " " << last[i].first << " " << last[i].second
                          << std::endl;
            }
            for (int i = 0; i < candidate_result.size(); ++i) {
                LOG(INFO) << "Product " << candidate_result[i]->toString() << std::endl;
            }
        }
        for (auto& example: example_list) assert(task->evaluate(candidate_result, example));
        auto example = verifyOccam(candidate_result);
        if (example.first) addCounterExample(example);
        else {
            recorder::recorder.addRecord("#turn", turn_num);
            recordRes(candidate_result, turn_num, vis_f, tot);
            return candidate_result;
        }
        if (config::KIsVerbose) {
            LOG(INFO) << util::dataList2String(*example.first) << " " << util::dataList2String(*example.second)
                      << std::endl;
        }
    }
}