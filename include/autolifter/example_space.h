//
// Created by pro on 2020/11/24.
//

#ifndef CPP_EXAMPLE_SPACE_H
#define CPP_EXAMPLE_SPACE_H

#include "basic/data.h"
#include <functional>
#include <random>

typedef DataList Example;

struct ExampleInfo {
    std::string feature;
    Data info;
    Example* example;
};

struct ExampleSpaceConfig {
    int int_min = config::KIntMin;
    int int_max = config::KIntMax;
    int min_length = 0;
    int max_length = 10;
    int init_size = 10000;
    int increase_rate = 3;
    bool is_full = false;
    std::function<bool(const DataList&)> verifier = [](const DataList&)->bool{return true;};
};

class ExampleSpace {
public:
    std::vector<Example*> input_space;
    std::vector<Type> type_list;
    ExampleSpaceConfig c;
    explicit ExampleSpace(const std::vector<Type>& _type_list, const ExampleSpaceConfig& _c): type_list(_type_list), c(_c) {}
    virtual std::string getName() = 0;
    virtual void acquireMoreExamples(int target_num = -1) = 0;
};

class RandomExampleSpace: public ExampleSpace {
    std::discrete_distribution<int> l_gen, int_gen;
    void addNextExample();
public:
    RandomExampleSpace(const std::vector<Type>& type_list, const ExampleSpaceConfig& c);
    virtual std::string getName() {return "random";}
    void acquireMoreExamples(int target_num);
};

class FullExampleSpace: public ExampleSpace {
public:
    FullExampleSpace(const std::vector<Type>& type_list, const ExampleSpaceConfig& c);
    virtual std::string getName() {return "full";}
    void acquireMoreExamples(int target_num) {}
};

namespace example_space {
    extern ExampleSpace* buildExample(const std::vector<Type>& type_list, const ExampleSpaceConfig& c);
}

#endif //CPP_EXAMPLE_SPACE_H
