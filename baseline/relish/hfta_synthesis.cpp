//
// Created by pro on 2022/11/8.
//

#include "baseline/relish.h"
#include "basic/util.h"
#include "basic/semantics_factory.h"
#include <set>
#include <queue>
#include <unordered_set>
#include "glog/logging.h"

namespace tmp {
    class FTAEdge;

    class FTANode {
    public:
        NonTerminal* symbol;
        int size;
        DataList oup_list;
        std::vector<FTAEdge*> edge_list;
        int tag;
        Data getOutput(int id);
        FTANode(NonTerminal* _symbol, int _size, const DataList& _oup): symbol(_symbol), size(_size), oup_list(_oup) {};
        ~FTANode() {
            for (auto* edge: edge_list) delete edge;
        }
    };

    class CrossSemantics: public ParamSemantics {
    public:
        FTANode* sub_node;
        CrossSemantics(FTANode* _sub_node, ParamSemantics* semantics): ParamSemantics(semantics->getId(), semantics->getType()), sub_node(_sub_node) {}
    };

    class FTAEdge {
    public:
        Semantics* semantics;
        std::vector<FTANode*> node_list;
        int tag, num;
        FTAEdge(Semantics* _semantics, const std::vector<FTANode*> _node_list):
            semantics(_semantics), node_list(_node_list) {
        }
    };

    Program* buildProgram(FTANode* node) {
        assert(node->edge_list.size());
        ProgramList sub_list;
        for (auto* sub_node: node->edge_list[0]->node_list) {
            sub_list.push_back(buildProgram(sub_node));
        }
        return new Program(sub_list, node->edge_list[0]->semantics);
    }

    bool isConst(Program* p) {
        if (dynamic_cast<ParamSemantics*>(p->semantics)) return false;
        for (auto* sub: p->sub_list) {
            if (!isConst(sub)) return false;
        }
        return true;
    }

    Data FTANode::getOutput(int id) {
        if (symbol->type != TSEMANTICS) return oup_list[id];
        auto* program = buildProgram(this);
        assert(isConst(program));
        return program->run(DataList());
    }

    int getInpNum(Task* task, int num) {
        return task->task_type->const_num + task->task_type->arity * (num + 1);
    }

    class FTA {
    public:
        std::unordered_map<std::string, std::vector<FTANode*>> node_map;
        std::vector<FTANode*> final_list;
        Grammar* g;
        int num;
        void clear() {
            for (auto& item: node_map)
                for (auto *node: item.second) node->tag = false;
        }
        FTA(const std::unordered_map<std::string, std::vector<FTANode*>> &_node_map, Grammar* _g, int _num, int size_limit, bool is_equal): g(_g), node_map(_node_map) {
            int start_id = g->start_symbol->id; num = _num;
            for (auto& item: node_map) {
                for (auto* node: item.second) {
                    if (node->symbol->id == start_id && (!is_equal || node->size == size_limit)) {
                        final_list.push_back(node);
                    }
                }
            }
            clear();
            for (auto* node: final_list) node->tag = true;
            removeUseless();
        }
        std::vector<FTANode*> removeUselessWithoutDelete();
        void removeUseless() {
            auto node_list = removeUselessWithoutDelete();
            for (auto* node: node_list) delete node;
        }
        ~FTA() {
            for (auto& item: node_map) {
                for (auto* node: item.second) delete node;
            }
        }
    };


    std::vector<FTANode *> FTA::removeUselessWithoutDelete() {
        std::queue<FTANode *> Q;
        for (auto *node: final_list) if (node->tag) Q.push(node);
        while (!Q.empty()) {
            auto *node = Q.front();
            for (auto *edge: node->edge_list) {
                for (auto *sub_node: edge->node_list) {
                    if (sub_node->tag) continue;
                    Q.push(sub_node);
                    sub_node->tag = true;
                }
            }
            Q.pop();
        }
        int now = 0;
        for (auto *node: final_list) {
            if (node->tag) final_list[now++] = node;
        }
        final_list.resize(now);
        std::vector<FTANode *> useless;
        for (auto &item: node_map) {
            now = 0;
            for (auto *node: item.second) {
                if (node->tag) item.second[now++] = node;
                else useless.push_back(node);
            }
            item.second.resize(now);
        }
        return useless;
    }

    std::string _getListFeature(int depth, int id) {
        return std::to_string(depth) + "@" + std::to_string(id);
    }

    void _collect(int k, std::vector<FTANode*>& tmp, const std::vector<std::vector<FTANode*>>& storage, std::vector<std::vector<FTANode*>>& res) {
        if (k == storage.size()) {
            res.push_back(tmp); return;
        }
        for (auto* node: storage[k]) {
            tmp[k] = node; _collect(k + 1, tmp, storage, res);
        }
    }

    std::vector<std::vector<FTANode*>> _flatten(const std::vector<std::vector<FTANode*>>& storage) {
        std::vector<FTANode*> tmp(storage.size());
        std::vector<std::vector<FTANode*>> res;
        _collect(0, tmp, storage, res);
        return res;
    }

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

    FTA* constructFTA(Grammar* grammar, int size_limit, const std::vector<std::vector<FTANode*>>& node_list, bool is_equal) {
        grammar->indexSymbols();
        std::unordered_map<std::string, std::vector<FTANode*>> node_map;
        int dummy = 0;
        for (int size = 1; size <= size_limit; ++size) {
            for (auto* symbol: grammar->symbol_list) {
                std::unordered_map<std::string, FTANode*> new_node_map;

                auto get_node = [&](const Data& oup) {
                    auto feature = oup.toString();
                    if (new_node_map.count(feature)) return new_node_map[feature];
                    return new_node_map[feature] = new FTANode(symbol, size, {oup});
                };

                for (auto* rule: symbol->rule_list) {
                    std::vector<std::vector<int>> size_storage(rule->param_list.size());
                    for (int i = 0; i < rule->param_list.size(); ++i) {
                        for (int j = 0; j < size; ++j) {
                            auto feature = _getListFeature(j, rule->param_list[i]->id);
                            if (node_map.count(feature) && !node_map[feature].empty()) {
                                size_storage[i].push_back(j);
                            }
                        }
                    }
                    auto* ps = dynamic_cast<ParamSemantics*>(rule->semantics);
                    if (ps) {
                        if (size != 1) continue;
                        for (auto* node: node_list[ps->getId()]) {
                            auto oup = node->oup_list[0];
                            auto* new_node = get_node(oup);
                            auto* edge = new FTAEdge(new CrossSemantics(node, ps), {});
                            new_node->edge_list.push_back(edge);
                        }
                        continue;
                    }
                    auto size_split = splitSize(size - 1, size_storage);
                    for (auto size_scheme: size_split) {
                        std::vector<std::vector<FTANode *>> sub_storage(rule->param_list.size());
                        for (int i = 0; i < rule->param_list.size(); ++i) {
                            sub_storage[i] = node_map[_getListFeature(size_scheme[i], rule->param_list[i]->id)];
                        }
                        for (auto &sub_list: sub_storage) if (sub_list.empty()) continue;
                        auto sub_scheme = _flatten(sub_storage);
                        for (auto &scheme: sub_scheme) {
                            DataList sub(scheme.size());
                            for (int i = 0; i < scheme.size(); ++i) sub[i] = scheme[i]->getOutput(0);
                            Data oup;
                            std::string oup_feature;
                            if (symbol->type == TSEMANTICS) {
                                oup = semantics::curry(rule->semantics, sub, false);
                                oup_feature = std::to_string(++dummy);
                            } else {
                                oup = rule->semantics->run(std::move(sub), nullptr);
                                oup_feature = oup.toString();
                            }
                            FTANode *new_node;
                            if (!new_node_map.count(oup_feature))
                                new_node = new_node_map[oup_feature] = new FTANode(symbol, size, {oup});
                            else new_node = new_node_map[oup_feature];
                            auto *edge = new FTAEdge(rule->semantics, scheme);
                            new_node->edge_list.push_back(edge);
                            // LOG(INFO) << "build " << size << " " << symbol->name << " " << grammar->start_symbol->name << " " << util::dataList2String(new_node->oup_list) << " " << buildProgram(new_node)->toString();
                        }
                    }
                }
                auto& new_node = node_map[_getListFeature(size, symbol->id)];
                for (auto& item: new_node_map) new_node.push_back(item.second);
            }
        }
        return new FTA(node_map, grammar, 1, size_limit, is_equal);
    }

    std::pair<std::string, std::string> _getSplitFeature(const DataList& oup_list, int l_size) {
        DataList l, r;
        for (int i = 0; i < l_size; ++i) l.push_back(oup_list[i]);
        for (int i = l_size; i < oup_list.size(); ++i) r.push_back(oup_list[i]);
        return {util::dataList2String(l), util::dataList2String(r)};
    }

    FTA* mergeFTA(FTA* l, FTA* r, int size_limit, const std::vector<std::vector<FTANode*>>& node_list, bool is_equal) {
        auto* grammar = l->g; int dummy = 0; int num = l->num + r->num;
        std::unordered_map<std::string, std::vector<FTANode*>> node_map;
        for (int size = 1; size <= size_limit; ++size) {
            for (auto* symbol: grammar->symbol_list) {
                auto list_feature = _getListFeature(size, symbol->id);
                auto& l_list = l->node_map[list_feature], &r_list = r->node_map[list_feature];
                if (l_list.empty() || r_list.empty()) continue;

                std::unordered_set<std::string> l_valid, r_valid;
                for (auto* node: l_list) l_valid.insert(util::dataList2String(node->oup_list));
                for (auto* node: r_list) r_valid.insert(util::dataList2String(node->oup_list));

                /*LOG(INFO) << "valid for " << size << " " << size_limit << " " << symbol->name;
                std::cout << "  L:";
                for (auto* node: l_list) std::cout << "  " << util::dataList2String(node->oup_list) << std::endl;
                std::cout << "  R:";
                for (auto* node: r_list) std::cout << "  " << util::dataList2String(node->oup_list) << std::endl;*/

                auto is_valid = [&](const DataList& oup_list) {
                    auto sub_feature = _getSplitFeature(oup_list, l->num);
                    return !(l_valid.find(sub_feature.first) == l_valid.end() ||
                             r_valid.find(sub_feature.second) == r_valid.end());
                };

                std::unordered_map<std::string, FTANode*> new_node_map;

                auto get_node = [&](const DataList& oup_list) {
                    auto feature = util::dataList2String(oup_list);
                    if (new_node_map.count(feature)) return new_node_map[feature];
                    return new_node_map[feature] = new FTANode(symbol, size, oup_list);
                };

                for (auto* rule: symbol->rule_list) {
                    auto* ps = dynamic_cast<ParamSemantics*>(rule->semantics);
                    if (ps) {
                        if (size > 1) continue;
                        for (auto* node: node_list[ps->getId()]) {
                            auto oup_list = node->oup_list;
                            if (!is_valid(oup_list)) continue;
                            auto* new_node = get_node(oup_list);
                            auto* edge = new FTAEdge(new CrossSemantics(node, ps), {});
                            new_node->edge_list.push_back(edge);
                        }
                        continue;
                    }

                    std::vector<std::vector<int>> size_storage(rule->param_list.size());
                    for (int i = 0; i < rule->param_list.size(); ++i) {
                        for (int j = 0; j < size; ++j) {
                            auto feature = _getListFeature(j, rule->param_list[i]->id);
                            if (node_map.count(feature) && !node_map[feature].empty()) size_storage[i].push_back(j);
                        }
                    }
                    auto size_split = splitSize(size - 1, size_storage);
                    for (auto size_scheme: size_split) {
                        std::vector<std::vector<FTANode *>> sub_storage(rule->param_list.size());
                        for (int i = 0; i < rule->param_list.size(); ++i) {
                            sub_storage[i] = node_map[_getListFeature(size_scheme[i], rule->param_list[i]->id)];
                        }
                        for (auto &sub_list: sub_storage) if (sub_list.empty()) continue;
                        auto sub_scheme = _flatten(sub_storage);
                        for (auto &scheme: sub_scheme) {
                            FTANode *new_node;
                            if (symbol->type == TSEMANTICS) {
                                auto feature = std::to_string(++dummy);
                                new_node = new_node_map[feature] = new FTANode(symbol, size, {});
                            } else {
                                DataList oup_list(num);
                                for (int i = 0; i < num; ++i) {
                                    DataList sub(scheme.size());
                                    for (int j = 0; j < scheme.size(); ++j)
                                        sub[j] = scheme[j]->getOutput(i);
                                    oup_list[i] = rule->semantics->run(std::move(sub), nullptr);
                                }
                                if (!is_valid(oup_list)) continue;
                                auto oup_feature = util::dataList2String(oup_list);
                                if (!new_node_map.count(oup_feature))
                                    new_node = new_node_map[oup_feature] = new FTANode(symbol, size, oup_list);
                                else new_node = new_node_map[oup_feature];
                            }
                            auto *edge = new FTAEdge(rule->semantics, scheme);
                            new_node->edge_list.push_back(edge);
                        }
                    }
                }
                auto& new_node = node_map[_getListFeature(size, symbol->id)];
                for (auto& item: new_node_map) new_node.push_back(item.second);
            }
        }
        return new FTA(node_map, grammar, num, size_limit, is_equal);
    }

    class HFTA {
    public:
        std::vector<FTANode*> list_inputs, int_inputs;
        std::vector<FTA*> aux_fta_list;
        FTA* comb_fta;
        int aux_size_limit, comb_size_limit;
        HFTA(const std::vector<FTANode*>& _list_inputs, const std::vector<FTANode*>& _int_inputs, const std::vector<FTA*>& _aux_fta_list, FTA* _comb_fta, int _aux_size, int _comb_size):
            list_inputs(_list_inputs), int_inputs(_int_inputs), aux_fta_list(_aux_fta_list), comb_fta(_comb_fta), aux_size_limit(_aux_size), comb_size_limit(_comb_size) {
            for (auto* fta: aux_fta_list) fta->clear();
            for (auto& node_list: comb_fta->node_map) {
                for (auto* node: node_list.second) {
                    for (auto* edge: node->edge_list) {
                        auto* cs = dynamic_cast<CrossSemantics*>(edge->semantics);
                        if (cs) cs->sub_node->tag = true;
                    }
                }
            }
            for (auto* fta: aux_fta_list) fta->removeUseless();
        }
        ~HFTA() {
            for (auto* node: list_inputs) delete node;
            for (auto* node: int_inputs) delete node;
            delete comb_fta;
            for (auto* fta: aux_fta_list) delete fta;
        }
    };

    struct FTAExample {
        DataList lists, values;
        Data oup;
    };

    void _flattenProgram(int pos, ProgramList& tmp, const ProgramStorage& ps, ProgramStorage& res) {
        if (pos == ps.size()) {
            res.push_back(tmp); return;
        }
        for (auto& p: ps[pos]) {
            tmp[pos] = p;
            _flattenProgram(pos + 1, tmp, ps, res);
        }
    }

    ProgramStorage flattenProgram(const ProgramStorage& ps) {
        ProgramList tmp(ps.size());
        ProgramStorage res;
        _flattenProgram(0, tmp, ps, res);
        return res;
    }

    ProgramList collectAll(FTANode* node) {
        ProgramList res;
        for (auto* edge: node->edge_list) {
            auto* ps = dynamic_cast<ParamSemantics*>(edge->semantics);
            if (ps && ps->getId() == 0) continue;
            if (dynamic_cast<ConstSemantics*>(edge->semantics)) continue;
            ProgramStorage sub_res;
            for (auto* sub_node: edge->node_list) {
                sub_res.push_back(collectAll(sub_node));
            }
            for (auto& now: flattenProgram(sub_res)) res.push_back(new Program(now, edge->semantics));
        }
        return res;
    }

    HFTA* constructHFTA(Grammar* aux, Grammar* comb, const FTAExample& example, int aux_size, int comb_size) {
        std::vector<FTA*> aux_fta_list;

        std::vector<FTANode*> list_inputs(example.lists.size()), int_inputs(example.values.size());
        for (int i = 0; i < example.lists.size(); ++i) {
            auto* node = new FTANode(nullptr, 1, {example.lists[i]});
            list_inputs[i] = node;
        }
        for (int i = 0; i < example.values.size(); ++i) {
            auto* node = new FTANode(nullptr, 1, {example.values[i]});
            int_inputs[i] = node;
        }

        for (int i = 0; i < example.lists.size(); ++i) {
            auto* fta = constructFTA(aux, aux_size, {{list_inputs[i]}}, false);
            aux_fta_list.push_back(fta);
        }

        std::vector<std::vector<FTANode*>> mid_list;
        for (auto* fta: aux_fta_list) {
            mid_list.push_back(fta->final_list);
        }
        for (auto* value: int_inputs) {
            mid_list.push_back({value});
        }
        auto* comb_fta = constructFTA(comb, comb_size, mid_list, true);
        comb_fta->clear();
        for (auto* node: comb_fta->final_list) {
            if (node->oup_list[0] == example.oup) {
                node->tag = true;
            }
        }
        comb_fta->removeUseless();

        /*LOG(INFO) << "construct" << std::endl;
        for (auto& node: comb_fta->final_list) {
            for (auto& prog: collectAll(node)) std::cout << "  " << prog->toString() << std::endl;
        }*/
        return new HFTA(list_inputs, int_inputs, aux_fta_list, comb_fta, aux_size, comb_size);
    }

    DataList _mergeDataList(const DataList& x, const DataList& y) {
        DataList res = x;
        for (const auto& v: y) res.push_back(v);
        return res;
    }

    HFTA* mergeHFTA(HFTA* l, HFTA* r) {
        std::vector<FTANode*> list_inputs, int_inputs;
        assert(l->list_inputs.size() == r->list_inputs.size());
        for (int i = 0 ; i < l->list_inputs.size(); ++i) {
            DataList value_list = _mergeDataList(l->list_inputs[i]->oup_list, r->list_inputs[i]->oup_list);
            list_inputs.push_back(new FTANode(nullptr, 1, value_list));
        }
        assert(l->int_inputs.size() == r->int_inputs.size());
        for (int i = 0; i < l->int_inputs.size(); ++i) {
            DataList value_list = _mergeDataList(l->int_inputs[i]->oup_list, r->int_inputs[i]->oup_list);
            int_inputs.push_back(new FTANode(nullptr, 1, value_list));
        }
        std::vector<FTA*> aux_fta_list;
        for (int i = 0; i < l->list_inputs.size(); ++i) {
            aux_fta_list.push_back(mergeFTA(l->aux_fta_list[i], r->aux_fta_list[i], l->aux_size_limit, {{list_inputs[i]}}, false));
        }

        std::vector<std::vector<FTANode*>> mid_list;
        for (auto* fta: aux_fta_list) {
            mid_list.push_back(fta->final_list);
        }
        for (auto* value: int_inputs) {
            mid_list.push_back({value});
        }
        auto* comb_fta = mergeFTA(l->comb_fta, r->comb_fta, l->comb_size_limit, mid_list, true);
        comb_fta->clear();
        DataList target_oup_list;
        if (!l->comb_fta->final_list.empty() && !r->comb_fta->final_list.empty()) {
            target_oup_list = _mergeDataList(l->comb_fta->final_list[0]->oup_list, r->comb_fta->final_list[0]->oup_list);
        }
        for (auto* node: comb_fta->final_list) {
            if (node->oup_list == target_oup_list) node->tag = true;
        }
        comb_fta->removeUseless();
        /*LOG(INFO) << "merge" << std::endl;
        for (auto& node: comb_fta->final_list) {
            for (auto& prog: collectAll(node)) std::cout << "  " << prog->toString() << std::endl;
        }*/
        return new HFTA(list_inputs, int_inputs, aux_fta_list, comb_fta, l->aux_size_limit, l->comb_size_limit);
    }

    struct FTAResult {
        std::vector<Program*> aux_list;
        Program* merge;
    };

    void printAllProgram(FTA* fta) {
        std::cout << "print" << std::endl;
        for (int i = 0; i < fta->final_list.size(); ++i) {
            std::cout << "  " << buildProgram(fta->final_list[i])->toString() << std::endl;
        }
    }

    std::vector<Program*> collectAux(HFTA* hfta) {
        std::vector<Program*> aux_pool;
        int root_num = hfta->aux_fta_list.size();
        for (int S = 1; S < (1 << root_num); ++S) {
            FTA* root = nullptr;
            DataList inp_list;
            for (int i = 0; i < (hfta->aux_fta_list.size()); ++i) {
                if (S & (1 << i)) {
                    inp_list = _mergeDataList(inp_list, hfta->list_inputs[i]->oup_list);
                    if (!root) root = hfta->aux_fta_list[i];
                    else {
                        auto* pre = root;
                        auto *inp_node = new FTANode(nullptr, 1, inp_list);
                        root = mergeFTA(root, hfta->aux_fta_list[i], hfta->aux_size_limit, {{inp_node}}, false);
                    }
                }
            }
            for (auto* node: root->final_list) {
                aux_pool.push_back(buildProgram(node));
            }
        }
        return aux_pool;
    }

    Program* constructProgram(FTANode* node, int sign, const std::unordered_map<FTANode*, int>& inp_node_map, int fta_size, int aux_num) {
        auto it = inp_node_map.find(node);
        if (it != inp_node_map.end()) {
            return new Program({}, new ParamSemantics(it->second, node->symbol->type));
        }
        for (auto* edge: node->edge_list) {
            auto* cs = dynamic_cast<CrossSemantics*>(edge->semantics);
            if (cs) {
                int id = cs->getId();
                if (id >= fta_size) {
                    id += (aux_num - 1) * fta_size;
                    return new Program({}, new ParamSemantics(id, node->symbol->type));
                }
                continue;
            }
            bool is_valid = true;
            for (auto* sub_node: edge->node_list) {
                if (sub_node->tag != sign) is_valid = false;
            }
            if (is_valid) {
                ProgramList sub_list;
                for (auto* sub_node: edge->node_list) {
                    sub_list.push_back(constructProgram(sub_node, sign, inp_node_map, fta_size, aux_num));
                }
                return new Program(sub_list, edge->semantics);
            }
        }
        assert(0);
    }

    bool extractFTAResult(HFTA* hfta, FTAResult& res, int aux_size, int aux_num) {
        auto aux_pool = collectAux(hfta);

        int sign = 0;
        auto* comb_fta = hfta->comb_fta;

        std::unordered_map<FTANode*, std::vector<std::pair<FTAEdge*, FTANode*>>> rev_edge_map;
        std::vector<std::unordered_map<std::string, std::vector<FTANode*>>> cross_node_pool(hfta->list_inputs.size());
        std::vector<FTANode*> const_node_list;
        std::unordered_set<FTANode*> final_set;
        for (auto* node: comb_fta->final_list) final_set.insert(node);
        for (auto& item: comb_fta->node_map) {
            for (auto* node: item.second) {
                node->tag = 0;
                for (auto* edge: node->edge_list) {
                    for (auto* sub_node: edge->node_list) {
                        rev_edge_map[sub_node].emplace_back(edge, node);
                    }
                    auto* cs = dynamic_cast<CrossSemantics*>(edge->semantics);
                    if (cs) {
                        if (cs->getId() < cross_node_pool.size())
                            cross_node_pool[cs->getId()][util::dataList2String(node->oup_list)].push_back(node);
                        else const_node_list.push_back(node);
                    } else {
                        assert(!dynamic_cast<ParamSemantics*>(edge->semantics));
                        if (edge->node_list.empty()) const_node_list.push_back(node);
                    }
                    edge->tag = 0;
                }
            }
        }

        auto update_edge = [&](FTAEdge* edge) {
            if (edge->tag != sign) edge->tag = sign, edge->num = edge->node_list.size();
            return --edge->num;
        };

        // todo: extend to more than 1 auxilary program
        std::vector<std::vector<Program*>> aux_plans;
        if (aux_size == 1 && aux_num == 1) aux_plans.push_back({});

        for (auto* aux: aux_pool) aux_plans.push_back({aux});
        for (int i = 0; i < aux_pool.size(); ++i) {
            for (int j = i + 1; j < aux_pool.size(); ++j) {
                aux_plans.push_back({aux_pool[i], aux_pool[j]});
            }
        }
        for (auto& aux_list: aux_plans) {
            std::unordered_map<FTANode*, int> aux_inp_node_map;
            int id = -1;
            for (int i = 0; i < hfta->list_inputs.size(); ++i) {
                for (auto* aux: aux_list) {
                    DataList oup_list; ++id;
                    for (auto& l: hfta->list_inputs[i]->oup_list) {
                        oup_list.push_back(aux->run({l}));
                    }
                    auto feature = util::dataList2String(oup_list);
                    if (cross_node_pool[i].count(feature)) {
                        for (auto *node: cross_node_pool[i][feature]) {
                            aux_inp_node_map[node] = id;
                        }
                    }
                }
            }

            sign++;
            std::queue<FTANode*> Q;
            auto insert = [&](FTANode* node) {
                if (node->tag == sign) return;
                Q.push(node); node->tag = sign;
            };

            for (auto& start: aux_inp_node_map) insert(start.first);
            for (auto c_node: const_node_list) insert(c_node);

            while (!Q.empty()) {
                auto* node = Q.front(); Q.pop();
                if (final_set.find(node) != final_set.end()) {
                    LOG(INFO) << "extract " << hfta->aux_fta_list.size() << " " << aux_list.size();
                    auto comb = constructProgram(node, sign, aux_inp_node_map, hfta->aux_fta_list.size(), aux_list.size());
                    LOG(INFO) << "found: ";
                    std::cout << "aux: [";
                    for (auto aux: aux_list) std::cout << aux->toString() << ",";
                    std::cout << "], comb: " << comb->toString() << std::endl;
                    res.aux_list = aux_list; res.merge = comb;
                    return true;
                }
                for (auto& [rev_edge, source_node]: rev_edge_map[node]) {
                    if (!update_edge(rev_edge)) insert(source_node);
                }
            }
        }
        return false;
    }

    Grammar * buildLiftingGrammar(Task* task) {
        auto* grammar = grammar::getDeepCoderDSL(true);
        grammar->addParam({TLIST});
        std::set<std::string> semantics_set;
        for (auto &extra_semantics: task->extra_semantics) {
            extra_semantics.insertExtraSemantics(grammar);
        }
        return grammar;
    }

    Grammar* buildCombinatorGrammar(Task* task, int size) {
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
        expr->rule_list.push_back(new Rule(semantics::getSemanticsFromName("ite"), {cmp, expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<="), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {expr, expr}));
        cmp->rule_list.push_back(new Rule(semantics::getSemanticsFromName("="), {expr, def_value}));
        auto* grammar = new Grammar(expr, {cmp, atom, expr, def_value});
        for (auto &extra_semantics: task->extra_sygus_semantics) {
            extra_semantics.insertExternalExtraSemantics(grammar);
        }
        return grammar;
    }

    FTAExample buildExample(Example* example, Task* task) {
        DataList list_inputs, int_inputs = task->executeProgramOnInput(task->target, *example);
        for (int i = 0; i < task->task_type->const_num; ++i) int_inputs.push_back(example->at(i));
        for (int i = 0; i < task->task_type->arity; ++i) {
            auto index = i + task->task_type->const_num;
            list_inputs.push_back(example->at(index));
        }
        Data oup = task->executeTargetAfterOp(*example);
        return {list_inputs, int_inputs, oup};
    }

    Data run(const FTAResult& result, const FTAExample& example) {
        DataList input;
        for (auto& v: example.lists) {
            for (auto* program: result.aux_list) {
                input.push_back(program->run({v}));
            }
        }
        for (auto& v: example.values) input.push_back(v);
        return result.merge->run(input);
    }

    bool getExample(const FTAResult& result, const std::vector<FTAExample>& example_list, FTAExample& res) {
        for (auto& example: example_list) {
            if (run(result, example) != example.oup) {
                res = example; return true;
            }
        }
        return false;
    }

    struct WorkSpace {
        HFTA* root;
        int example_num;
        int aux_limit, comb_limit;
        Grammar *aux, *comb;
        WorkSpace(int _aux_limit, int _comb_limit, Grammar* _aux, Grammar* _comb): aux_limit(_aux_limit), comb_limit(_comb_limit), aux(_aux), comb(_comb) {
            root = nullptr; example_num = 0;
        }
        bool isEmpty() {
            if (!root) return false;
            return root->comb_fta->final_list.empty();
        }
        void addExample(const FTAExample& example) {
             if (isEmpty()) return;
             auto* hfta = constructHFTA(aux, comb, example, aux_limit, comb_limit);
             if (!root) root = hfta;
             else {
                 for (auto* fta: root->aux_fta_list) assert(fta->g == aux);
                 auto* pre = root;
                 root = mergeHFTA(root, hfta);
                 delete pre; delete hfta;
             }
        }
        void update(const std::vector<FTAExample>& example_list) {
            for (int i = example_num; i < example_list.size(); ++i) {
                addExample(example_list[i]);
            }
            example_num = example_list.size();
        }
        bool getResult(FTAResult& res) {
            for (auto* fta: root->aux_fta_list) assert(fta->g == aux);
            auto flag =  extractFTAResult(root, res, aux_limit, 1);
            for (auto* fta: root->aux_fta_list) assert(fta->g == aux);
            return flag;
        }
    };
}

std::pair<std::vector<Program *>, Program *> hfta::HFTASynthesis(Task *task) {
    std::cout << task->name << std::endl;
    int example_num = 10000;
    task->example_space->acquireMoreExamples(example_num);
    std::vector<tmp::FTAExample> example_space;
    for (int i = 0; i < example_num; ++i) {
        example_space.push_back(tmp::buildExample(task->example_space->input_space[i], task));
    }

    auto* aux_grammar = tmp::buildLiftingGrammar(task);
    int inp_num = tmp::getInpNum(task, 1);
    auto* comb_grammar = tmp::buildCombinatorGrammar(task, inp_num);
    LOG(INFO) << "start grammar " << aux_grammar << " " << comb_grammar;

    std::vector<tmp::FTAExample> example_list;
    std::vector<int> comb_size_limit(1);
    std::vector<tmp::WorkSpace*> space_list;
    int turn_id = -1;
    auto relax = [&]() {
        turn_id += 1;
        for (int i = 1; i < comb_size_limit.size(); ++i) comb_size_limit[i]++;
        if (turn_id % 2 == 0) comb_size_limit.push_back(4);
        for (auto* space: space_list) delete space;
        space_list.resize(comb_size_limit.size(), nullptr);
        for (int i = 1; i < space_list.size(); ++i) {
            space_list[i] = new tmp::WorkSpace(i, comb_size_limit[i], aux_grammar, comb_grammar);
        }
    };

    example_list.push_back(example_space[0]);
    tmp::FTAResult result;
    while (1) {
        bool is_found = false;
        while (!is_found) {
            for (int i = 1; i < space_list.size(); ++i) {
                space_list[i]->update(example_list);
                if (space_list[i]->getResult(result)) {
                    is_found = true; break;
                }
            }
            if (is_found) break;
            relax();
        }
        tmp::FTAExample counter_example;
        LOG(INFO) << "current result";
        for (auto* prog: result.aux_list) std::cout << "  " << prog->toString(); std::cout << std::endl;
        std::cout << "  " << result.merge->toString() << std::endl;
        if (!tmp::getExample(result, example_space, counter_example)) break;
        LOG(INFO) << "example " << util::dataList2String(counter_example.values) << "@" << util::dataList2String(counter_example.lists) << " -> " << counter_example.oup.toString();
        example_list.push_back(counter_example);
    }
    std::cout << "aux:" << std::endl;
    for (auto& p: result.aux_list) std::cout << "  " << p->toString();
    std::cout << "comb:" << std::endl;
    return {result.aux_list, result.merge};
}