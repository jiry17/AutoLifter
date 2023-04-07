//
// Created by pro on 2021/6/26.
//

#include "benchmark.h"
#include "semantics_factory.h"
#include <map>

namespace {
    typedef std::function<Program*()> OperatorBuilder;
}

namespace op_pool {
    Program* plusBuilder() {
        auto* list_param = new Program({}, new ParamSemantics(1, TLIST));
        auto* int_param = new Program({}, new ParamSemantics(0, TINT));
        auto plus = [](DataList&& inp, GlobalInfo* info) -> Data{
            auto l(inp[1].moveList());
            int x = inp[0].getInt();
            for (auto& v: l) v += x;
            return new ListValue(l);
        };
        return new Program({int_param, list_param}, new AnonymousSemantics(plus, {TINT, TLIST}, TLIST, "plus"));
    }

    Program* coverBuilder() {
        auto* list_param = new Program({}, new ParamSemantics(1, TLIST));
        auto* int_param = new Program({}, new ParamSemantics(0, TINT));
        auto cover = [](DataList&& inp, GlobalInfo* info) -> Data {
            auto l(inp[1].moveList());
            int x = inp[0].getInt();
            for (auto& v: l) v = x;
            return new ListValue(l);
        };
        return new Program({int_param, list_param}, new AnonymousSemantics(cover, {TINT, TLIST}, TLIST, "cover"));
    }

    Program* negBuilder() {
        auto* list_param = new Program({}, new ParamSemantics(1, TLIST));
        auto neg = [](DataList&& inp, GlobalInfo* info) -> Data {
            auto l(inp[0].moveList());
            for (auto &v: l) v = -v;
            return new ListValue(l);
        };
        return new Program({list_param}, new AnonymousSemantics(neg, {TLIST}, TLIST, "neg"));
    }

    std::map<std::string, OperatorBuilder> op_builder_map = {
            {"+", plusBuilder}, {"cover", coverBuilder}, {"-", negBuilder}
    };

    Program* getOperator(const std::string& name) {
        return op_builder_map[name]();
    }
}

namespace query_pool {
    typedef std::function<SemanticsFunction()> QueryBuilder;

    SemanticsFunction sumBuilder() {
        return [](DataList&& inp, GlobalInfo*) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int &v: l) ans += v;
            return ans;
        };
    }

    SemanticsFunction sqrBuilder() {
        return [](DataList&& inp, GlobalInfo*) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int &v: l) ans += v * v;
            return ans;
        };
    }

    SemanticsFunction mpsBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            int sum = 0, mps = -config::KDefaultValue;
            auto list = inp[0].moveList();
            for (auto v: list) {
                sum += v;
                mps = std::max(mps, sum);
            }
            return mps;
        };
    }

    SemanticsFunction mtsBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            int mts = -config::KDefaultValue;
            auto list = inp[0].moveList();
            for (auto v: list) {
                mts = std::max(0, mts) + v;
            }
            return mts;
        };
    }

    SemanticsFunction mssBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            int mss = 0, mts = 0;
            auto list = inp[0].moveList();
            for (auto v: list) {
                mts = std::max(mts + v, 0);
                mss = std::max(mss, mts);
            }
            return mss;
        };
    }

    SemanticsFunction minBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            auto list(inp[0].moveList());
            int mi = config::KDefaultValue;
            for (auto v: list) mi = std::min(mi, v);
            return mi;
        };
    }

    SemanticsFunction secondMinBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            auto list(inp[0].moveList());
            int mi = config::KDefaultValue, second_mi = config::KDefaultValue;
            for (auto v: list) {
                if (v < mi) {
                    second_mi = mi;
                    mi = v;
                } else second_mi = std::min(second_mi, v);
            }
            return second_mi;
        };
    }

    SemanticsFunction thirdMinBuilder() {
        return [](DataList&& inp, GlobalInfo* info) {
            auto list(inp[0].moveList());
            if (list.size() <= 2) return config::KDefaultValue;
            std::sort(list.begin(), list.end());
            return list[2];
        };
    }

    SemanticsFunction max1sBuilder() {
        return [](DataList&& inp, GlobalInfo* global_info)->Data {
            auto list(inp[0].moveList());
            int pre = 0, result = 0;
            for (int i = 0; i <= list.size(); ++i) {
                if (i == list.size() || list[i] == 0) {
                    int current_size = i - pre;
                    result = std::max(result, current_size);
                    pre = i + 1;
                }
            }
            return result;
        };
    }

    SemanticsFunction max1sPosBuilder() {
        return [](DataList&& inp, GlobalInfo* global_info) -> Data {
            auto list(inp[0].moveList());
            int pre = 0, result = 0, pos = 0;
            for (int i = 0; i <= list.size(); ++i) {
                if (i == list.size() || list[i] == 0) {
                    int current_size = i - pre;
                    if (current_size > result) {
                        result = current_size;
                        pos = pre;
                    }
                    pre = i + 1;
                }
            }
            return pos;
        };
    }

    std::map<std::string, QueryBuilder> query_builder_map = {
            {"sum", sumBuilder}, {"sqrsum", sqrBuilder}, {"mps", mpsBuilder},
            {"mss", mssBuilder}, {"mts", mtsBuilder}, {"min", minBuilder},
            {"2nd-min", secondMinBuilder}, {"3rd-min", thirdMinBuilder},
            {"max-1s", max1sBuilder}, {"max-1s-p", max1sPosBuilder}
    };

    SemanticsFunction getQuery(const std::string& name) {
        return query_builder_map[name]();
    }
}

namespace {
    std::pair<std::string, std::string> splitName(const std::string& name) {
        std::string query_name, op_name;
        bool is_occur = false;
        for (char c: name) {
            if (c == '@') is_occur = true;
            else if (is_occur) op_name += c;
            else query_name += c;
        }
        return {query_name, op_name};
    }
}

namespace extra_pool{
    struct ExtraInfo {
        std::vector<ExtraSemantics> extra_tree;
        std::vector<ExternalExtraSemantics> external_extra_tree;
        ExampleSpaceConfig c_tree;
        std::vector<ExtraSemantics> extra_op;
        std::vector<ExternalExtraSemantics> external_extra_op;
        ExampleSpaceConfig c_op;
    };

    typedef std::function<ExtraInfo()> ExtraInfoBuilder;

    ExtraInfo defaultExtraInfo() {
        ExampleSpaceConfig c_tree, c_op;
        c_tree.min_length = 1;
        c_op.min_length = 1;
        return {{}, {}, c_tree, {}, {}, c_op};
    }

    ExtraInfo timesBuilder() {
        auto info = defaultExtraInfo();
        info.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        return info;
    }

    ExtraInfo mssBuilder() {
        auto info = defaultExtraInfo();
        auto mss = query_pool::getQuery("mss");
        auto mss_semantics = new AnonymousSemantics(mss, {TLIST}, TINT, "mss");
        info.extra_op.emplace_back(mss_semantics);
        info.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        return info;
    }

    ExtraInfo max1sBuilder() {
        auto info = defaultExtraInfo();
        info.c_op.int_min = info.c_tree.int_min = 0;
        info.c_op.int_max = info.c_tree.int_max = 1;
        return info;
    }

    ExtraInfo max1sPosBuilder() {
        auto info = max1sBuilder();
        auto max_1s = query_pool::getQuery("max-1s");
        info.extra_tree.emplace_back(new AnonymousSemantics(max_1s, {TLIST}, TINT, "max_1s"));
        info.extra_op.emplace_back(new AnonymousSemantics(max_1s, {TLIST}, TINT, "max_1s"));
        return info;
    }

    std::map<std::string, ExtraInfoBuilder> extra_info_map = {
            {"sum@+", timesBuilder}, {"sum@cover", timesBuilder},
            {"sqrsum@+", timesBuilder}, {"sqrsum@cover", timesBuilder},
            {"mps@-", defaultExtraInfo}, {"mps@cover", timesBuilder},
            {"mts@-", defaultExtraInfo}, {"mts@cover", timesBuilder},
            {"mss@-", mssBuilder}, {"mss@cover", mssBuilder},
            {"min@-", defaultExtraInfo}, {"2nd-min@-", defaultExtraInfo},
            {"3rd-min@-", defaultExtraInfo}, {"max-1s@cover", max1sBuilder},
            {"max-1s-p@cover", max1sPosBuilder}
    };

    ExtraInfo getExtraInfo(const std::string& name) {
        return extra_info_map[name]();
    }
}

std::vector<Task *> benchmark::getSegmentTree(const std::string &name) {
    auto split_result = splitName(name);
    auto query_name = split_result.first, op_name = split_result.second;
    auto query = query_pool::getQuery(query_name);
    auto* op = op_pool::getOperator(op_name);
    auto extra_info = extra_pool::getExtraInfo(name);
    auto* task_tree = benchmark::defaultBuilder("tree", query_name, query, extra_info.c_tree, extra_info.extra_tree, extra_info.external_extra_tree);
    auto* task_op = benchmark::defaultBuilder("operator", query_name, query, extra_info.c_op, extra_info.extra_op,
            extra_info.external_extra_op, {{"operator", op}});
    return {task_tree, task_op};
}

