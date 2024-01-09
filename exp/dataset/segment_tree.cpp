//
// Created by pro on 2021/6/26.
//

#include "exp/benchmark.h"
#include "basic/semantics_factory.h"
#include <map>
#include "glog/logging.h"

namespace {
    typedef std::function<Program*()> OperatorBuilder;

    struct OperatorInfo {
        std::string init_tag, tag_merger;
        Semantics* tag_applier;
    };
}

namespace op_pool {
    OperatorInfo plusBuilder() {
        auto plus = [](DataList&& inp, GlobalInfo* info) -> Data{
            auto l(inp[1].moveList());
            int x = inp[0].getInt();
            for (auto& v: l) v += x;
            return new ListValue(l);
        };
        return {"0", "tag[pos] + a", new AnonymousSemantics(plus, {TINT, TLIST}, TLIST, "plus")};
    }

    OperatorInfo coverBuilder() {
        auto cover = [](DataList&& inp, GlobalInfo* info) -> Data {
            auto l(inp[1].moveList());
            int x = inp[0].getInt();
            for (auto& v: l) v = x;
            return new ListValue(l);
        };
        return {"KINF", "a", new AnonymousSemantics(cover, {TINT, TLIST}, TLIST, "cover")};
    }

    OperatorInfo negBuilder() {
        auto neg = [](DataList&& inp, GlobalInfo* info) -> Data {
            auto l(inp[1].moveList());
            for (auto &v: l) v = -v;
            return new ListValue(l);
        };
        return {"0", "tag[pos]?0:1", new AnonymousSemantics(neg, {TINT, TLIST}, TLIST, "neg")};
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

    SemanticsFunction sqrsumBuilder() {
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

    SemanticsFunction second_minBuilder() {
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

    SemanticsFunction third_minBuilder() {
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

    SemanticsFunction max1s_pBuilder() {
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
}

namespace {
    struct BenchmarkConfig {
        std::vector<ExtraSemantics> extra_tree;
        std::vector<ExternalExtraSemantics> external_extra_tree;
        ExampleSpaceConfig c_tree;
        std::vector<ExtraSemantics> extra_op;
        std::vector<ExternalExtraSemantics> external_extra_op;
        ExampleSpaceConfig c_op;
    };

    BenchmarkConfig defaultExtraInfo() {
        ExampleSpaceConfig c_tree, c_op;
        c_tree.min_length = 1;
        c_op.min_length = 1;
        return {{}, {}, c_tree, {}, {}, c_op};
    }

#define BuildSegmentTreeBenchmark(query_name, op_name) \
    auto op_info = op_pool::op_name ## Builder(); \
    auto* paradigm = new SegmentTreeParadigm(op_info.init_tag, op_info.tag_merger, op_info.tag_applier); \
    auto task_types = paradigm->getTaskTypes(); \
    auto semantics = query_pool::query_name ## Builder();                                                                          \
    auto task_name = #query_name "-" #op_name;                                                                                  \
    auto task_merge = benchmark::defaultBuilder("dac", task_types[0], task_name, \
                                                semantics, config.c_tree, config.extra_tree, config.external_extra_tree); \
    auto task_op = benchmark::defaultBuilder("op", task_types[1], task_name, \
                                             semantics, config.c_op, config.extra_op, config.external_extra_op); \
    auto source_path = config::KSourcePath + "resource/dataset/segment_tree/" + task_name + ".cpp"; \
    return {paradigm, {task_merge, task_op}, source_path}

    BenchmarkInfo sum_plusBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(sum, plus);
    }

    BenchmarkInfo sum_coverBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(sum, cover);
    }

    BenchmarkInfo sqrsum_plusBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(sqrsum, plus);
    }

    BenchmarkInfo sqrsum_coverBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(sqrsum, cover);
    }

    BenchmarkInfo mps_negBuilder() {
        auto config = defaultExtraInfo();
        BuildSegmentTreeBenchmark(mps, neg);
    }

    BenchmarkInfo mps_coverBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(mps, cover);
    }

    BenchmarkInfo mts_negBuilder() {
        auto config = defaultExtraInfo();
        BuildSegmentTreeBenchmark(mts, neg);
    }

    BenchmarkInfo mts_coverBuilder() {
        auto config = defaultExtraInfo();
        config.external_extra_op.emplace_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(mts, cover);
    }

    BenchmarkInfo mss_negBuilder() {
        auto config = defaultExtraInfo();
        auto mss_semantics = new AnonymousSemantics(query_pool::mssBuilder(), {TLIST}, TINT, "mss");
        config.extra_op.emplace_back(mss_semantics);
        BuildSegmentTreeBenchmark(mss, neg);
    }

    BenchmarkInfo mss_coverBuilder() {
        auto config = defaultExtraInfo();
        auto mss_semantics = new AnonymousSemantics(query_pool::mssBuilder(), {TLIST}, TINT, "mss");
        config.extra_op.emplace_back(mss_semantics);
        config.external_extra_op.push_back(semantics::getSemanticsFromName("*"));
        BuildSegmentTreeBenchmark(mss, neg);
    }

    BenchmarkInfo min_negBuilder() {
        auto config = defaultExtraInfo();
        BuildSegmentTreeBenchmark(min, neg);
    }

    BenchmarkInfo second_min_negBuilder() {
        auto config = defaultExtraInfo();
        BuildSegmentTreeBenchmark(second_min, neg);
    }

    BenchmarkInfo third_min_negBuilder() {
        auto config = defaultExtraInfo();
        BuildSegmentTreeBenchmark(third_min, neg);
    }

    BenchmarkInfo max1s_coverBuilder() {
        auto config = defaultExtraInfo();
        config.c_op.int_min = config.c_tree.int_min = 0;
        config.c_op.int_max = config.c_tree.int_max = 1;
        BuildSegmentTreeBenchmark(max1s, cover);
    }

    BenchmarkInfo max1s_p_coverBuilder() {
        auto config = defaultExtraInfo();
        config.c_op.int_min = config.c_tree.int_min = 0;
        config.c_op.int_max = config.c_tree.int_max = 1;
        auto max_1s = query_pool::max1sBuilder();
        config.extra_tree.emplace_back(new AnonymousSemantics(max_1s, {TLIST}, TINT, "max_1s"));
        config.extra_op.emplace_back(new AnonymousSemantics(max_1s, {TLIST}, TINT, "max_1s"));
        BuildSegmentTreeBenchmark(max1s_p, cover);
    }

#define Register(query_name, op_name) {#query_name "-" #op_name, query_name ## _ ## op_name ##Builder}
    std::map<std::string, std::function<BenchmarkInfo()>> benchmark_map = {
        Register(sum, plus), Register(sum, cover), Register(sqrsum, plus), Register(sqrsum, cover),
        Register(mts, neg), Register(mts, cover), Register(mss, neg), Register(mss, cover),
        Register(min, neg), Register(second_min, neg), Register(third_min, neg),
        Register(max1s, cover), Register(max1s_p, cover)
    };
}

BenchmarkInfo benchmark::getSegmentTree(const std::string &name) {
    if (benchmark_map.find(name) == benchmark_map.end()) {
        LOG(FATAL) << "Unknown benchmark " << name;
    }
    return benchmark_map[name]();
}
