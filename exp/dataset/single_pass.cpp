//
// Created by pro on 2021/6/24.
//

#include "basic/semantics.h"
#include "exp/benchmark.h"
#include "basic/semantics_factory.h"
#include "basic/util.h"
#include "glog/logging.h"
#include <regex>
#include "glog/logging.h"

namespace {
    ExampleSpaceConfig defaultExampleConfig() {
        ExampleSpaceConfig c;
        c.min_length = 1;
        return c;
    }
}

#define BuildSinglePassTask(name, example_space, external, extra) \
    auto paradigm = new SinglePassParadigm(); \
    auto task_types = paradigm->getTaskTypes();      \
    auto* task = benchmark::defaultBuilder("single-pass", task_types[0], name, semantics, example_space, external, extra); \
    auto source_path = config::KSourcePath + "resource/dataset/single_pass/" + name + ".cpp"; \
    return {paradigm, {task}, source_path, {}}

#define BuildDefaultSinglePassTask(name) \
    auto config = defaultExampleConfig(); \
    BuildSinglePassTask(name, config, {}, {})

namespace {

    namespace pldi17 {
        BenchmarkInfo sumBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                auto list = inp[0].moveList();
                int ans = 0;
                for (auto v: list) ans += v;
                return ans;
            };
            BuildDefaultSinglePassTask("sum");
        }

        BenchmarkInfo minBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                auto list = inp[0].moveList();
                int ans = config::KDefaultValue;
                for (auto v: list) ans = std::min(ans, v);
                return ans;
            };
            BuildDefaultSinglePassTask("min");
        }

        BenchmarkInfo maxBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                auto list = inp[0].moveList();
                int ans = -config::KDefaultValue;
                for (auto v: list) ans = std::max(ans, v);
                return ans;
            };
            BuildDefaultSinglePassTask("max");
        }

        BenchmarkInfo averageBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                auto list = inp[0].moveList();
                int sum = 0;
                for (int v: list) sum += v;
                return sum / int(list.size());
            };
            auto div = [](DataList &&inp, GlobalInfo *info) {
                int x = inp[0].getInt(), y = inp[1].getInt();
                if (y == 0) return config::KExampleIntMax;
                return x / y;
            };
            auto *div_semantics = new AnonymousSemantics(div, {TINT, TINT}, TINT, "div");
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 20;
            config::KMaxBranchNum = 1;
            std::vector<ExternalExtraSemantics> extra_list;
            extra_list.push_back(div_semantics);
            extra_list.push_back(semantics::getSemanticsFromName("+"));
            extra_list.push_back(new ConstSemantics(1));
            BuildSinglePassTask("average", c, {}, extra_list);
        }

        BenchmarkInfo lengthBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int l = 0;
                auto list = inp[0].moveList();
                for (auto v: list) ++l;
                return l;
            };
            BuildDefaultSinglePassTask("length");
        }

        BenchmarkInfo secondMinBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int min_value = config::KDefaultValue;
                int second_min_value = config::KDefaultValue;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    if (v < min_value) {
                        second_min_value = min_value;
                        min_value = v;
                    } else second_min_value = std::min(second_min_value, v);
                }
                return second_min_value;
            };
            BuildDefaultSinglePassTask("2nd-min");
        }

        BenchmarkInfo mpsBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int sum = 0, mps = 0;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    sum += v;
                    mps = std::max(mps, sum);
                }
                return mps;
            };
            BuildDefaultSinglePassTask("mps");
        }

        BenchmarkInfo mtsBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int mts = 0;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    mts = std::max(0, mts + v);
                }
                return mts;
            };
            BuildDefaultSinglePassTask("mts");
        }

        BenchmarkInfo mssBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int mss = 0, mts = 0;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    mts = std::max(mts + v, 0);
                    mss = std::max(mss, mts);
                }
                return mss;
            };
            BuildDefaultSinglePassTask("mss");
        }

        BenchmarkInfo mpsPosBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int mps = 0, pos = 0, sum = 0;
                auto list = inp[0].moveList();
                for (int i = 0; i < list.size(); ++i) {
                    sum += list[i];
                    if (sum > mps) pos = i;
                    mps = std::max(mps, sum);
                }
                return pos;
            };
            BuildDefaultSinglePassTask("mps_p");
        }

        BenchmarkInfo mtsPosBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int pos = -1, mts = 0;
                auto list = inp[0].moveList();
                for (int i = 0; i < list.size(); ++i) {
                    if (mts + list[i] < 0) pos = i;
                    mts = std::max(0, mts + list[i]);
                }
                return pos;
            };
            BuildDefaultSinglePassTask("mts_p");
        }

        BenchmarkInfo isSortedBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int result = 1;
                int pre = -config::KDefaultValue;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    result = result & (pre < v);
                    pre = v;
                }
                return result;
            };
            BuildDefaultSinglePassTask("is_sorted");
        }

        BenchmarkInfo atoiBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int result = 0;
                auto list(inp[0].moveList());
                for (int v: list) result = result * 10 + v;
                return result;
            };
            auto c = defaultExampleConfig();
            std::vector<ExternalExtraSemantics> extra_list;
            extra_list.emplace_back(semantics::getSemanticsFromName("*"));
            extra_list.emplace_back(new ConstSemantics(10));
            c.int_min = 0;
            c.int_max = 9;
            c.max_length = 3;
            BuildSinglePassTask("atoi", c, {}, extra_list);
        }

        BenchmarkInfo dropWhileBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                auto list = inp[0].moveList();
                for (int i = 0; i < list.size(); ++i) {
                    if (list[i]) return i;
                }
                return int(list.size());
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("dropwhile", c, {}, {});
        }

        BenchmarkInfo balancedBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int cnt = 0;
                auto list = inp[0].moveList();
                for (int v: list) {
                    cnt += v;
                    if (cnt < 0) return 0;
                }
                return 1;
            };
            auto c = defaultExampleConfig();
            c.int_min = -1;
            c.int_max = 1;
            BuildSinglePassTask("balanced", c, {}, {});
        }

        BenchmarkInfo zeroStartOneStarBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int an = 1, bn = 1;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    an = v && an;
                    bn = ((!v) || an) && bn;
                }
                return bn;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("0s1s", c, {}, {});
        }

        BenchmarkInfo cntOnesBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int i = 0, f = 0, cnt = 0;
                auto a = inp[0].moveList();
                int n = a.size();
                for (i = 0; i < n; i++) {
                    cnt += (a[i] && !f) ? 1 : 0;
                    f = a[i];
                }
                return cnt;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("cnt_1s", c, {}, {});
        }

        BenchmarkInfo lineSightBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int ma = 0, vis = 1;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    vis = (ma <= v);
                    ma = std::max(ma, v);
                }
                return vis;
            };
            BuildDefaultSinglePassTask("line_sight");
        }

        BenchmarkInfo maxLenOnesBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int ma = 0, l = 0;
                auto list = inp[0].getList();
                for (int i = 0; i <= list.size(); ++i) {
                    if (i == list.size() || list[i] == 0) {
                        ma = std::max(ma, l);
                        l = 0;
                    } else ++l;
                }
                return ma;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("max_len_1s", c, {}, {});
        }

        BenchmarkInfo zeroAfterOneBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) -> Data {
                bool seen1 = false, res = false;
                auto l(inp[0].getList());
                for (int i = 0; i < l.size(); ++i) {
                    if (seen1 && !(l[i])) res = true;
                    seen1 = seen1 || l[i];
                }
                return new IntValue(res);
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("0after1", c, {}, {});
        }
    }

    namespace pldi21 {
        BenchmarkInfo count10PlusBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                int result = 0;
                LIST list = inp[0].moveList();
                for (int i = 1; i < list.size(); ++i) {
                    if (list[i] == 0) result += list[i - 1];
                }
                return result;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("count10p", c, {}, {});
        }

        BenchmarkInfo count10star2Builder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                bool s0 = false;
                int c = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    c += s0 && (list[i] == 2 ? 1 : 0);
                    s0 = (list[i] == 1) || (s0 && list[i] == 0);
                }
                return c;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 5;
            BuildSinglePassTask("count10s2", c, {}, {});
        }

        BenchmarkInfo count1star2star3starBuilder() {
            // Such an implementation actually count 1+2*3+
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                bool s1 = false, s2 = false;
                int result = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    result += ((list[i] == 3 && (s2 || s1)) ? 1 : 0);
                    s2 = (list[i] == 2) && (s1 || s2);
                    s1 = (list[i] == 1);
                }
                return result;
            };
            auto c = defaultExampleConfig();
            c.int_min = 1;
            c.int_max = 3;
            BuildSinglePassTask("count1s2s3s", c, {}, {});
        }

        BenchmarkInfo maxSumBetweenOnesBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                int ms = 0, cs = 0;
                LIST list = inp[0].moveList();
                for (int i = 0; i < list.size(); ++i) {
                    cs = list[i] != 1 ? cs + list[i] : 0;
                    ms = std::max(ms, cs);
                }
                return ms;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 3;
            BuildSinglePassTask("max_sum_between_ones", c, {}, {});
        }

        BenchmarkInfo maxDistanceBetweenZerosBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                int md = 0, cd = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    cd = list[i] ? cd + 1 : 0;
                    md = std::max(md, cd);
                }
                return md;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("max_dist_between_zeros", c, {}, {});
        }

        BenchmarkInfo lisBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                LIST list = inp[0].getList();
                int cl = 0, ml = 0, prev = list[0];
                for (int i = 1; i < list.size(); ++i) {
                    cl = prev < list[i] ? cl + 1 : 0;
                    ml = std::max(ml, cl);
                    prev = list[i];
                }
                return ml;
            };
            BuildDefaultSinglePassTask("lis");
        }

        BenchmarkInfo largestPeakBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                LIST list = inp[0].getList();
                int cmo = 0, lpeak = 0;
                for (int i = 0; i < list.size(); ++i) {
                    cmo = list[i] > 0 ? cmo + list[i] : 0;
                    lpeak = std::max(cmo, lpeak);
                }
                return lpeak;
            };
            BuildDefaultSinglePassTask("largest_peak");
        }

        BenchmarkInfo longest1starBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                int ml = 0, len = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    len = list[i] == 1 ? len + 1 : 0;
                    ml = std::max(ml, len);
                }
                return ml;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("longest1s", c, {}, {});
        }

        BenchmarkInfo longest10star2Builder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                bool s0 = false, s1;
                int ml = 0, len = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    s1 = s0 && (list[i] == 2);
                    s0 = (list[i] == 1) || (list[i] == 0 && s0);
                    len = (s1 || s0) ? len + 1 : 0;
                    ml = s1 ? std::max(ml, len) : ml;
                    if (s1) len = 0;
                    if (list[i] == 1) len = 1;
                }
                return ml;
            };
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 2;
            BuildSinglePassTask("longest10s2", c, {}, {});
        }

        BenchmarkInfo longestEven0starBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                int cl = 0, ml = 0, ml_tmp = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    cl = list[i] == 0 ? cl + 1 : 0;
                    ml_tmp = std::max(ml_tmp, cl);
                    if (ml_tmp % 2 == 0) ml = ml_tmp;
                }
                return ml;
            };
            auto is_even = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                return inp[0].getInt() % 2;
            };
            ExternalExtraSemantics is_even_info(new AnonymousSemantics(is_even, {TINT}, TINT, "is_even"));
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            config::KMaxBranchNum = 5;
            BuildSinglePassTask("longest00s", c, {}, { is_even_info });
        }

        BenchmarkInfo longestOdd10starBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                bool s1 = false, s2 = false;
                int cl = 0, ml = 0;
                LIST list = inp[0].getList();
                for (int i = 0; i < list.size(); ++i) {
                    s1 = (s2 && list[i] == 1);
                    s2 = (list[i] == 0);
                    cl = (s1 ? cl + 1 : (s2 ? cl : 0));
                    ml = (cl % 2 == 1 ? std::max(ml, cl) : ml);
                }
                return ml;
            };
            auto is_even = [](DataList &&inp, GlobalInfo *global_info) -> Data {
                return inp[0].getInt() % 2;
            };
            ExternalExtraSemantics is_even_info(new AnonymousSemantics(is_even, {TINT}, TINT, "is_even"), true);
            auto c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            c.max_length = 15;
            c.init_size *= 10;
            BuildSinglePassTask("longest_odd10s", c, {}, { is_even_info });
        }
    }

    namespace extra {
        BenchmarkInfo thirdMinBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *info) {
                int min_value = config::KDefaultValue;
                int second_min_value = config::KDefaultValue;
                int third_min_value = config::KDefaultValue;
                auto list = inp[0].moveList();
                for (auto v: list) {
                    if (v < min_value) {
                        third_min_value = second_min_value;
                        second_min_value = min_value;
                        min_value = v;
                    } else if (v < second_min_value) {
                        third_min_value = second_min_value;
                        second_min_value = v;
                    } else third_min_value = std::min(third_min_value, v);
                }
                return third_min_value;
            };
            BuildDefaultSinglePassTask("3rd-min");
        }

        BenchmarkInfo mppBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int current = 1, ans = -config::KDefaultValue;
                for (int i = 0; i < list.size(); ++i) {
                    current *= list[i];
                    ans = std::max(ans, current);
                }
                return ans;
            };
            ExampleSpaceConfig c = defaultExampleConfig();
            c.max_length = 8;
            c.int_min = -3;
            c.int_max = 3;
            BuildSinglePassTask("mpp", c, {}, { semantics::getSemanticsFromName("*") });
        }

        BenchmarkInfo mtpBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int current = 1, ans = -config::KDefaultValue;
                std::reverse(list.begin(), list.end());
                for (int i = 0; i < list.size(); ++i) {
                    current *= list[i];
                    ans = std::max(ans, current);
                }
                return ans;
            };
            ExampleSpaceConfig c = defaultExampleConfig();
            c.max_length = 8;
            c.int_min = -3;
            c.int_max = 3;
            BuildSinglePassTask("mtp", c, {}, { semantics::getSemanticsFromName("*") });
        }

        BenchmarkInfo mspBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int ans = -config::KDefaultValue;
                for (int i = 0; i < list.size(); ++i) {
                    int current = 1;
                    for (int j = i; j < list.size(); ++j) {
                        current *= list[j];
                        ans = std::max(ans, current);
                    }
                }
                return ans;
            };
            ExampleSpaceConfig c = defaultExampleConfig();
            c.max_length = 8;
            c.int_min = -3;
            c.int_max = 3;
            BuildSinglePassTask("msp", c, {}, { semantics::getSemanticsFromName("*") });
        }

        BenchmarkInfo max1sPBuilder() {
            auto max_1s_semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
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
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) -> Data {
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
            ExampleSpaceConfig c = defaultExampleConfig();
            c.int_min = 0;
            c.int_max = 1;
            BuildSinglePassTask("max_1s_p", c, { new AnonymousSemantics(max_1s_semantics, {TLIST}, TINT, "max_1s") }, {});
        }
    }

    namespace oopsla {
        BenchmarkInfo misBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int p = 0, np = 0;
                for (int v: list) {
                    int prep = p;
                    p = np + v;
                    np = std::max(prep, np);
                }
                return std::max(p, np);
            };
            BuildDefaultSinglePassTask("mis");
        }

        BenchmarkInfo masBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int res = 0, p = 0, n = 0;
                for (int v: list) {
                    int prep = p;
                    p = std::max(n, 0) + v;
                    n = std::max(prep, 0) - v;
                    res = std::max(res, std::max(p, n));
                }
                return res;
            };
            BuildDefaultSinglePassTask("mas");
        }

        BenchmarkInfo mmmBuilder() {
            auto semantics = [](DataList &&inp, GlobalInfo *global_info) {
                auto list(inp[0].moveList());
                int p = 0, np = 0, z = 0;
                for (auto v: list) {
                    int prep = p, prez = z, prenp = np;
                    p = std::max(prez, prenp) + v;
                    np = std::max(prez, prep) - v;
                    z = std::max(prenp, prep);
                }
                return std::max(std::max(p, np), z);
            };
            BuildDefaultSinglePassTask("mmm");
        }
    }

#define Register(name, func) {name, func ## Builder}
#define DefaultRegister(name) Register(#name, name)
    using namespace pldi17;
    using namespace pldi21;
    using namespace oopsla;
    using namespace extra;

    std::map<std::string, std::function<BenchmarkInfo()>> benchmark_map = {
            DefaultRegister(sum), DefaultRegister(min), DefaultRegister(max),
            DefaultRegister(length), Register("2nd-min", secondMin), DefaultRegister(mps),
            DefaultRegister(mts), DefaultRegister(mss), Register("mps_p", mpsPos),
            Register("mts_p", mtsPos), Register("is_sorted", isSorted), DefaultRegister(atoi),
            Register("dropwhile", dropWhile), DefaultRegister(balanced), Register("0s1s", zeroStartOneStar),
            Register("cnt_1s", cntOnes), Register("0after1", zeroAfterOne), Register("line_sight", lineSight),
            Register("max_len_1s", maxLenOnes), DefaultRegister(average), Register("count10p", count10Plus),
            Register("count10s2", count10star2), Register("count1s2s3s", count1star2star3star),
            Register("max_sum_between_ones", maxSumBetweenOnes),
            Register("max_dist_between_zeros", maxDistanceBetweenZeros),
            DefaultRegister(lis), Register("largest_peak", largestPeak), Register("longest1s", longest1star),
            Register("longest10s2", longest10star2), Register("longest00s", longestEven0star),
            Register("longest_odd10s", longestOdd10star),
            Register("3rd-min", thirdMin), DefaultRegister(mpp), DefaultRegister(mtp), DefaultRegister(msp),
            Register("max_1s_p", max1sP), DefaultRegister(mis), DefaultRegister(mas), DefaultRegister(mmm)
    };
}

BenchmarkInfo benchmark::getSinglePass(const std::string &name) {
    if (benchmark_map.find(name) == benchmark_map.end()) {
        LOG(FATAL) << "Unknown benchmark name " << name;
    }
    return benchmark_map[name]();
}
