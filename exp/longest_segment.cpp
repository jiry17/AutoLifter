//
// Created by pro on 2021/6/25.
//

#include "benchmark.h"

// all benchmarks in this file are collected from Hans Zantema: Longest Segment Problems. Sci. Comput. Program. 18(1): 39-66 (1992)

namespace {
    ExampleSpaceConfig defaultExampleConfig() {
        return ExampleSpaceConfig();
    }
}

namespace tactic1 {
    // tactic list
    std::vector<Task*> page9_1_Builder() {
        auto page9_1_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            for (int i = 1; i < l.size(); ++i) {
                if (!(l[i - 1] < l[i])) return 0;
            }
            return 1;
        };
        return {benchmark::defaultBuilder("listr", "page9-1", page9_1_semantics, defaultExampleConfig())};
    }

    std::vector<Task*> page9_2_Builder() {
        auto page9_2_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            if (l.empty()) return 1;
            int mi = config::KDefaultValue;
            for (int v: l) mi = std::min(mi, v);
            return int(mi == l[0]);
        };
        return {benchmark::defaultBuilder("listr", "page9-2", page9_2_semantics, defaultExampleConfig())};
    }

    std::vector<Task*> page10_1_Builder() {
        auto page10_1_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            if (l.empty()) return 1;
            int mi = config::KDefaultValue;
            for (int v: l) mi = std::min(mi, v);
            for (int i = 1; i < l.size(); ++i) {
                if (l[i] - l[i - 1] >= 2) return 0;
            }
            return int(mi == l[0]);
        };
        return {benchmark::defaultBuilder("listr", "page10-1", page10_1_semantics, defaultExampleConfig())};
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

    std::vector<Task*> page12Builder() {
        auto page12_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto l = inp[0].getList();
            int sum = 0;
            for (int i = 0; i < l.size(); ++i) {
                sum += l[i];
            }
            return new IntValue(sum <= 0);
        };
        auto* task1 = benchmark::defaultBuilder("listr", "page12", page12_semantics, defaultExampleConfig());
        auto* task2 = benchmark::defaultBuilder("tail", "page12", page12_semantics, windowingExampleConfigBuilder(page12_semantics));
        return {task1, task2};
    }
}

namespace tactic3 {
    typedef std::function<bool(int, int)> RCmp;

    ExampleSpaceConfig partitionExampleConfigBuilder(const RCmp& cmp) {
        auto verifier = [cmp](const DataList& inp) {
            int x = inp[0].getInt();
            LIST l = inp[1].getList(), r = inp[2].getList();
            for (auto v: l) if (!(cmp(v, x))) return false;
            for (auto v: r) if (cmp(x, v)) return false;
            return true;
        };
        ExampleSpaceConfig c;
        c.verifier = verifier;
        c.int_min = -10;
        c.int_max = 10;
        c.max_length = 5;
        return c;
    }

    std::vector<Task*> page20Builder() {
        auto page20_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        auto cmp = [](int x, int y) {return x <= y;};
        auto c = partitionExampleConfigBuilder(cmp);
        return {benchmark::defaultBuilder("segment", "page20", page20_semantics, c)};
    }

    std::vector<Task*> page21Builder() {
        auto page21_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        auto cmp = [](int x, int y) {return x <= y;};
        auto c = partitionExampleConfigBuilder(cmp);
        auto minimal_pos = [](DataList&& inp, GlobalInfo* info) {
            auto l(inp[0].moveList());
            LIST res; int cur = config::KDefaultValue;
            for (int i = 0; i < l.size(); ++i) {
                cur = std::min(cur, l[i]);
                if (cur == l[i]) res.push_back(i);
            }
            return new ListValue(res);
        };
        auto* minimal_pos_semantics = new AnonymousSemantics(minimal_pos, {TLIST}, TLIST, "min_pos");
        return {benchmark::defaultBuilder("segment", "page21", page21_semantics, c, {minimal_pos_semantics})};
    }

    std::vector<Task*> page22_1_Builder() {
        auto page22_1_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        auto cmp = [](int x, int y) {return x <= y;};
        auto c = partitionExampleConfigBuilder(cmp);
        c.min_length = 1;
        return {benchmark::defaultBuilder("segment", "page22-1", page22_1_semantics, c)};
    }

    std::vector<Task*> page22_2_Builder() {
        auto page22_2_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        auto cmp = [](int x, int y) {return x >= y;};
        auto c = partitionExampleConfigBuilder(cmp);
        c.min_length = 1;
        return {benchmark::defaultBuilder("segment", "page22-1", page22_2_semantics, c)};
    }
}

namespace {

    std::map<std::string, BatchedTaskBuilder> benchmark_map = {
            {"page9-1", tactic1::page9_1_Builder}, {"page9-2", tactic1::page9_2_Builder},
            {"page10-1", tactic1::page10_1_Builder}, {"page12", tactic2::page12Builder},
            {"page20", tactic3::page20Builder}, {"page21", tactic3::page21Builder},
            {"page22-1", tactic3::page22_1_Builder}, {"page22-2", tactic3::page22_2_Builder}
    };
}

std::vector<Task *> benchmark::getLongestSegment(const std::string &name) {
    return benchmark_map[name]();
}