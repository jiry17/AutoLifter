//
// Created by pro on 2020/11/24.
//

#include "autolifter/example_space.h"

#include <algorithm>
#include <iostream>
#include <random>

using std::vector;

namespace {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::discrete_distribution<int> getGenerator(int n, bool is_size) {
        std::vector<double> prob;
        if (is_size) {
            for (int i = 0; i < n; ++i) prob.push_back(std::exp(-std::abs(n - i - 1)) + 0.01);
        } else {
            for (int i = 0; i < n; ++i) prob.push_back(1);
        }
        std::discrete_distribution<int> d(prob.begin(), prob.end());
        return d;
    }

    std::vector<LIST> getListPool(int l, int r, int mi, int ma) {
        std::vector<LIST> s = {{}}, result, pre;
        if (l == 0) result.emplace_back();
        for (int i = 1; i <= r; ++i) {
            pre = std::move(s); s.clear();
            for (auto list: pre) {
                for (int j = mi; j <= ma; ++j) {
                    list.push_back(j);
                    s.push_back(list);
                    list.pop_back();
                }
            }
            if (i >= l) {
                for (auto& list: s) result.emplace_back(list);
            }
        }
        return result;
    }

    void collectAll(int pos, const std::vector<Type>& type_list, DataList& inp, const std::vector<int>& inp_pool, const std::vector<LIST>& list_pool, std::vector<Example*>& input_space) {
        if (pos == type_list.size()) {
            input_space.push_back(new Example(inp));
            return;
        }
        if (type_list[pos] == TINT) {
            for (int v: inp_pool) {
                inp.emplace_back(v);
                collectAll(pos + 1, type_list, inp, inp_pool, list_pool, input_space);
                inp.pop_back();
            }
        } else {
            for (auto& l: list_pool) {
                inp.emplace_back(new ListValue(l));
                collectAll(pos + 1, type_list, inp, inp_pool, list_pool, input_space);
                inp.pop_back();
            }
        }
    }
}

FullExampleSpace::FullExampleSpace(const std::vector<Type> &_type_list, const ExampleSpaceConfig &_c):
    ExampleSpace(_type_list, _c) {
    std::vector<LIST> list_pool = getListPool(c.min_length, c.max_length, c.int_min, c.int_max);
    std::vector<int> int_pool;
    for (int i = c.int_min; i <= c.int_max; ++i) {
        int_pool.push_back(i);
    }
    DataList inp;
    collectAll(0, type_list, inp, int_pool, list_pool, input_space);
    std::random_shuffle(input_space.begin(), input_space.end());
}

RandomExampleSpace::RandomExampleSpace(const std::vector<Type> &_type_list, const ExampleSpaceConfig& _c):
    ExampleSpace(_type_list, _c), l_gen(getGenerator(c.max_length - c.min_length + 1, false)),
    int_gen(getGenerator(c.int_max - c.int_min + 1, false)) {
    for (int num = 0; num < c.init_size; ++num) {
        addNextExample();
    }
}

void RandomExampleSpace::acquireMoreExamples(int target_num) {
    if (target_num == -1) target_num = int(input_space.size()) * c.increase_rate;
    if (target_num <= input_space.size()) return;
    while (input_space.size() < target_num) {
        addNextExample();
    }
}

void RandomExampleSpace::addNextExample() {
    std::vector<int> size_list;
    for (auto& type: type_list) {
        if (type == TLIST) {
            size_list.push_back(l_gen(gen) + c.int_min);
        } else {
            size_list.push_back(-1);
        }
    }
    while (true) {
        DataList inp;
        for (int i = 0; i < type_list.size(); ++i) {
            auto type = type_list[i];
            if (type == TINT) {
                inp.push_back(int_gen(gen) + c.int_min);
            } else {
                std::vector<int> list;
                int len = l_gen(gen) + c.min_length;
                for (int i = 0; i < len; ++i) {
                    list.push_back(int_gen(gen) + c.int_min);
                }
                inp.emplace_back(new ListValue(list));
            }
        }
        if (c.verifier(inp)) {
            input_space.push_back(new DataList(std::move(inp)));
            break;
        }
    }
}


ExampleSpace* example_space::buildExample(const std::vector<Type>& type_list, const ExampleSpaceConfig& c) {
    if (c.is_full) {
        return new FullExampleSpace(type_list, c);
    } else {
        return new RandomExampleSpace(type_list, c);
    }
}