#include <iostream>

#include "basic/program.h"
#include "basic/config.h"

Data Program::run(GlobalInfo *inp) {
    DataList sub_expr;
    for (auto* sub_program: sub_list) {
        auto sub_result = std::move(sub_program->run(inp));
        if (sub_result.getType() == TNONE) return std::move(sub_result);
        sub_expr.emplace_back(std::move(sub_result));
    }
    if (sub_expr.size() == semantics->inp_type_list.size()) {
        return semantics->run(std::move(sub_expr), inp);
    } else {
        return semantics::curry(semantics, sub_expr);
    }
}

Data Program::run(const DataList &inp) {
    auto* param_info = new ParamInfo(inp);
    Data result = run(param_info);
    if (result.getType() == TNONE) {
        if (semantics->oup_type == TINT) return config::KDefaultValue;
        else return new BoolValue(false);
    }
    delete param_info;
    return std::move(result);
}

std::string Program::toString() {
    if (sub_list.size() == 0) return semantics->name;
    std::string result = "(" + semantics->name;
    for (auto* sub_expr: sub_list) {
        result += " " + sub_expr->toString();
    }
    return result + ")";
}

void Program::print() {
    std::cout << toString() << std::endl;
}

int Program::size() {
    assert(semantics);
    int ans = 1;
    for (auto* sub_program: sub_list) {
        ans += sub_program->size();
    }
    return ans;
}

int Program::depth() {
    assert(semantics);
    int ans = 0;
    for (auto* sub_program: sub_list) {
        ans = std::max(ans, sub_program->depth());
    }
    return ans + 1;
}