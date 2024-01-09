//
// Created by pro on 2021/6/25.
//

#include "exp/benchmark.h"
#include "glog/logging.h"

namespace {
    ExampleSpaceConfig defaultExampleConfig() {
        return ExampleSpaceConfig();
    }
}

#define BuildLSP1Task(name, example_space, external, extra) \
    auto paradigm = new LSP1Paradigm(); \
    auto task_types = paradigm->getTaskTypes();      \
    auto* task = benchmark::defaultBuilder("lsp1", task_types[0], name, semantics, example_space, external, extra); \
    auto source_path = config::KSourcePath + "resource/dataset/lsp1/" + name + ".cpp"; \
    return {paradigm, {task}, source_path, {}}

namespace tactic1 {
    // tactic list
    BenchmarkInfo page9_1_Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            for (int i = 1; i < l.size(); ++i) {
                if (!(l[i - 1] < l[i])) return 0;
            }
            return 1;
        };
        BuildLSP1Task("page9-1", defaultExampleConfig(), {}, {});
    }

    BenchmarkInfo page9_2_Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            if (l.empty()) return 1;
            int mi = config::KDefaultValue;
            for (int v: l) mi = std::min(mi, v);
            return int(mi == l[0]);
        };
        BuildLSP1Task("page9-2", defaultExampleConfig(), {}, {});
    }

    BenchmarkInfo page10_1_Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            if (l.empty()) return 1;
            int mi = config::KDefaultValue;
            for (int v: l) mi = std::min(mi, v);
            for (int i = 1; i < l.size(); ++i) {
                if (l[i] - l[i - 1] >= 2) return 0;
            }
            return int(mi == l[0]);
        };
        BuildLSP1Task("page10-1", defaultExampleConfig(), {}, {});
    }
}


namespace tactic2 {
    ExampleSpaceConfig windowingExampleConfigBuilder(const SemanticsFunction& f ) {
        auto verifier = [f](const DataList& inp) {
            int c = inp[0].getInt();
            LIST l = inp[1].getList();
            if (l.empty() || c != l[0]) return false;
            return !f({inp[1]}, nullptr).getInt();
        };
        ExampleSpaceConfig c;
        c.verifier = verifier;
        return c;
    }

    BenchmarkInfo page12Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l = inp[0].getList();
            int sum = 0;
            for (int i = 0; i < l.size(); ++i) {
                sum += l[i];
            }
            return new IntValue(sum <= 10);
        };
        auto* paradigm = new LSP2Paradigm();
        auto task_types = paradigm->getTaskTypes();

        auto append_config = defaultExampleConfig();
        auto tail_config = windowingExampleConfigBuilder(semantics);
        append_config.int_min = 0; tail_config.int_min = 0;
        std::vector<ExternalExtraSemantics> extra_list;
        extra_list.emplace_back(new ConstSemantics(10));

        auto* append_task = benchmark::defaultBuilder("append", task_types[0], "page12", semantics, append_config, {}, extra_list);
        auto* tail_task = benchmark::defaultBuilder("tail", task_types[1], "page12", semantics, tail_config, {}, extra_list);
        auto source_path = config::KSourcePath + "resource/dataset/lsp2/page12.cpp";
        return {paradigm, {append_task, tail_task}, source_path};
    }
}

namespace tactic3 {
    typedef std::function<bool(int, int)> RCmp;

    std::tuple<ExampleSpaceConfig, ExampleSpaceConfig, ExampleSpaceConfig> partitionExampleConfigBuilder(const RCmp& cmp) {
        auto full_verifier = [cmp](const DataList& inp) {
            int x = inp[0].getInt();
            LIST l = inp[1].getList(), r = inp[2].getList();
            for (auto v: l) if (!(cmp(v, x))) return false;
            for (auto v: r) if (cmp(x, v)) return false;
            return true;
        };
        auto cons_verifier = [cmp](const DataList& inp) -> bool {
            int x = inp[0].getInt(); LIST r = inp[1].getList();
            for (auto v: r) if (cmp(x, v)) return false;
            return true;
        };
        auto append_verifier = [cmp](const DataList& inp) -> bool {
            int x = inp[0].getInt(); LIST l = inp[1].getList();
            for (auto v: l) if (!(cmp(v, x))) return false;
            return true;
        };
        ExampleSpaceConfig full_config;
        full_config.int_min = -10;
        full_config.int_max = 10;
        full_config.max_length = 5;
        full_config.min_length = 1;
        ExampleSpaceConfig cons_config = full_config, append_config = full_config;
        full_config.verifier = full_verifier;
        append_config.verifier = append_verifier;
        cons_config.verifier = cons_verifier;
        return {full_config, cons_config, append_config};
    }

#define DefineLSP3Task(name, op) \
    auto cmp = [](int x, int y) {return x op y;}; \
    auto* paradigm = new LSP3Paradigm(#op); \
    auto type_list = paradigm->getTaskTypes(); \
    auto [merge_config, cons_config, append_config] = partitionExampleConfigBuilder(cmp); \
    auto merge_task = benchmark::defaultBuilder("merge", type_list[0], name, semantics, merge_config, {}, {}); \
    auto cons_task = benchmark::defaultBuilder("cons", type_list[1], name, semantics, cons_config, {}, {}); \
    auto append_task = benchmark::defaultBuilder("append", type_list[2], name, semantics, append_config, {}, {}); \
    auto source_path = config::KSourcePath + "resource/dataset/lsp3/" + name + ".cpp"; \
    return {paradigm, {merge_task, cons_task, append_task}, source_path}

    BenchmarkInfo page20Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int i = 0; i < l.size() - ans; ++i) {
                int ma = -config::KDefaultValue;
                for (int j = i; j < l.size(); ++j) {
                    ma = std::max(ma, l[j]);
                    if (ma < j - i + 1) {
                        ans = std::max(j - i + 1, ans);
                    }
                }
            }
            return ans;
        };
        DefineLSP3Task("page20", <=);
    }

    BenchmarkInfo page21Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int i = 0; i < l.size() - ans; ++i) {
                int ma = -config::KDefaultValue;
                for (int j = i; j < l.size(); ++j) {
                    if (l[j] < l[i]) break;
                    ma = std::max(ma, l[j]);
                    if (ma == l[j]) ans = std::max(ans, j - i + 1);
                }
            }
            return ans;
        };
        DefineLSP3Task("page21", <=);
    }

    BenchmarkInfo page22_1_Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int i = 0; i < l.size() - ans; ++i) {
                int mi = config::KDefaultValue, ma = -config::KDefaultValue;
                for (int j = i; j < l.size(); ++j) {
                    mi = std::min(mi, l[j]);
                    ma = std::max(ma, l[j]);
                    if (mi + ma < j - i + 1) ans = std::max(ans, j - i + 1);
                }
            }
            return ans;
        };
        DefineLSP3Task("page22-1", <=);
    }

    BenchmarkInfo page22_2_Builder() {
        auto semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            int ans = 0;
            for (int i = 0; i < l.size() - ans; ++i) {
                int mi = config::KDefaultValue, ma = -config::KDefaultValue;
                for (int j = i; j < l.size(); ++j) {
                    mi = std::min(mi, l[j]);
                    ma = std::max(ma, l[j]);
                    if (mi + ma > j - i + 1) ans = std::max(ans, j - i + 1);
                }
            }
            return ans;
        };
        DefineLSP3Task("page22-2", >=);
    }
}

namespace {

    std::map<std::string, std::function<BenchmarkInfo()>> benchmark_map = {
            {"page9-1", tactic1::page9_1_Builder}, {"page9-2", tactic1::page9_2_Builder},
            {"page10-1", tactic1::page10_1_Builder}, {"page12", tactic2::page12Builder},
            {"page20", tactic3::page20Builder}, {"page21", tactic3::page21Builder},
            {"page22-1", tactic3::page22_1_Builder}, {"page22-2", tactic3::page22_2_Builder}
    };
}

BenchmarkInfo benchmark::getLongestSegment(const std::string &name) {
    if (benchmark_map.find(name) == benchmark_map.end()) {
        LOG(FATAL) << "Unknown benchmark name " << name;
    }
    return benchmark_map[name]();
}