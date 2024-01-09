//
// Created by pro on 2020/12/2.
//

#include "basic/recorder.h"
#include "basic/config.h"

#include <cassert>
#include <ctime>

namespace {
    double difftime(timeval start, timeval end) {
        long long second_diff = end.tv_sec - start.tv_sec;
        if (second_diff < 0) second_diff += 24 * 3600;
        long long u_diff = end.tv_usec - start.tv_usec;
        return second_diff + u_diff * 1e-6;
    }
}

void Recorder::popStage() {
#ifdef DEBUG
    assert(!stage_stack.empty());
#endif
    std::string stage_name = stage_stack[stage_stack.size() - 1];
    stage_stack.pop_back();
    timeval current_time;
    gettimeofday(&current_time, nullptr);
    time_cost[stage_name] += difftime(t, current_time);
    t = current_time;
}

void Recorder::pushStage(std::string stage_name) {
    timeval current_time;
    gettimeofday(&current_time, nullptr);
    if (!stage_stack.empty()) {
        std::string stage_name = stage_stack[stage_stack.size() - 1];
        time_cost[stage_name] += difftime(t, current_time);
    }
    stage_stack.push_back(stage_name);
    t = current_time;
}

void Recorder::addRecord(std::string name, int value) {
    info[name] += value;
}

Recorder::~Recorder() {
    double tot = 0.0;
    timeval fin;
    gettimeofday(&fin, nullptr);
    tot = difftime(init, fin);
    std::cout << "|" << std::setw(15) << " ";
    for (auto& cost: time_cost) std::cout << "| " << std::setw(10) << cost.first << " ";
    for (auto& info_item: info) std::cout << "| " << std::setw(10) << info_item.first << " ";
    std::cout << "|" << std::endl;
    std::cout << "|" << std::setw(15) << name;
    for (auto& cost: time_cost) std::cout << "| " << std::setw(10) << cost.second << " ";
    for (auto& info_item: info) std::cout << "| " << std::setw(10) << info_item.second << " ";
    std::cout << "|" << std::endl;
    std::cout << "Total time cost: " << tot << std::endl;
}

Recorder recorder::recorder;