//
// Created by pro on 2021/6/24.
//

#include "semantics.h"
#include "benchmark.h"
#include "semantics_factory.h"
#include "util.h"
#include "glog/logging.h"
#include <regex>

namespace {
    ExampleSpaceConfig defaultExampleConfig() {
        ExampleSpaceConfig c;
        c.min_length = 1;
        return c;
    }

    std::string tactic_name = "tree";
}

namespace pldi17{
    Task* sumBuilder() {
        auto sum_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto list = inp[0].moveList();
            int ans = 0;
            for (auto v: list) ans += v;
            return ans;
        };
        return benchmark::defaultBuilder(tactic_name, "sum", sum_semantics, defaultExampleConfig());
    }

    Task* minBuilder() {
        auto min_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto list = inp[0].moveList();
            int ans = config::KDefaultValue;
            for (auto v: list) ans = std::min(ans, v);
            return ans;
        };
        return benchmark::defaultBuilder(tactic_name, "min", min_semantics, defaultExampleConfig());
    }

    Task* maxBuilder() {
        auto max_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto list = inp[0].moveList();
            int ans = -config::KDefaultValue;
            for (auto v: list) ans = std::max(ans, v);
            return ans;
        };
        return benchmark::defaultBuilder(tactic_name, "max", max_semantics, defaultExampleConfig());
    }

    Task* averageBuilder() {
        auto average_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto list = inp[0].moveList();
            int sum = 0;
            for (int v: list) sum += v;
            return sum / int(list.size());
        };
        auto div = [](DataList&& inp, GlobalInfo* info) {
            int x = inp[0].getInt(), y = inp[1].getInt();
            if (y == 0) return config::KExampleIntMax;
            return x / y;
        };
        auto* div_semantics = new AnonymousSemantics(div, {TINT, TINT}, TINT, "div");
        auto plus = semantics::getSemanticsFromName("+");
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 20;
        config::KMaxBranchNum = 1;
        std::vector<ExternalExtraSemantics> extra_list;
        extra_list.push_back(div_semantics);
        extra_list.push_back(plus);
        if (benchmark::KIsSinglePass) {
            extra_list.push_back(new ConstSemantics(1));
        }
        return benchmark::defaultBuilder(tactic_name, "average", average_semantics, c, {},
                                         extra_list);
    }

    Task* lengthBuilder() {
        auto length_semantics = [](DataList&& inp, GlobalInfo* info) {
            int l = 0;
            auto list = inp[0].moveList();
            for (auto v: list) ++l;
            return l;
        };
        return benchmark::defaultBuilder(tactic_name, "length", length_semantics, defaultExampleConfig());
    }

    Task* secondMinBuilder() {
        auto second_min_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        return benchmark::defaultBuilder(tactic_name, "2nd-min", second_min_semantics, defaultExampleConfig());
    }

    Task* mpsBuilder() {
        auto mps_semantics = [](DataList&& inp, GlobalInfo* info) {
            int sum = 0, mps = 0;
            auto list = inp[0].moveList();
            for (auto v: list) {
                sum += v;
                mps = std::max(mps, sum);
            }
            return mps;
        };
        return benchmark::defaultBuilder(tactic_name, "mps", mps_semantics, defaultExampleConfig());
    }

    Task* mtsBuilder() {
        auto mts_semantics = [](DataList&& inp, GlobalInfo* info) {
            int mts = 0;
            auto list = inp[0].moveList();
            for (auto v: list) {
                mts = std::max(0, mts + v);
            }
            return mts;
        };
        return benchmark::defaultBuilder(tactic_name, "mts", mts_semantics, defaultExampleConfig());
    }

    Task* mssBuilder() {
        auto mss_semantics = [](DataList&& inp, GlobalInfo* info) {
            int mss = 0, mts = 0;
            auto list = inp[0].moveList();
            for (auto v: list) {
                mts = std::max(mts + v, 0);
                mss = std::max(mss, mts);
            }
            return mss;
        };
        return benchmark::defaultBuilder(tactic_name, "mss", mss_semantics, defaultExampleConfig(), {}, {}, {});
    }

    Task* mpsPosBuilder() {
        auto mps_pos_semantics = [](DataList&& inp, GlobalInfo* info) {
            int mps = 0, pos = 0, sum = 0;
            auto list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                sum += list[i];
                if (sum > mps) pos = i;
                mps = std::max(mps, sum);
            }
            return pos;
        };
        return benchmark::defaultBuilder(tactic_name, "mps_p", mps_pos_semantics, defaultExampleConfig());
    }

    Task* mtsPosBuilder() {
        auto mts_pos_semantics = [](DataList&& inp, GlobalInfo* info) {
            int pos = -1, mts = 0;
            auto list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                if (mts + list[i] < 0) pos = i;
                mts = std::max(0, mts + list[i]);
            }
            return pos;
        };
        return benchmark::defaultBuilder(tactic_name, "mts_p", mts_pos_semantics, defaultExampleConfig());
    }

    Task* isSortedBuilder() {
        auto is_sorted_semantics = [](DataList&& inp, GlobalInfo* info) {
            int result = 1;
            int pre = -config::KDefaultValue;
            auto list = inp[0].moveList();
            for (auto v: list) {
                result = result & (pre < v);
                pre = v;
            }
            return result;
        };
        return benchmark::defaultBuilder(tactic_name, "is_sorted", is_sorted_semantics, defaultExampleConfig());
    }

    Task* atoiBuilder() {
        auto atoi_semantics = [](DataList&& inp, GlobalInfo* info) {
            int result = 0;
            auto list(inp[0].moveList());
            for (int v: list) result = result * 10 + v;
            return result;
        };
        auto pow10 = [](DataList&& inp, GlobalInfo* info) {
            int n = inp[0].getInt();
            if (n >= 5) return config::KExampleIntMax;
            int res = 1;
            for (int i = 0; i < n; ++i) res *= 10;
            return res;
        };
        auto* pow10_info = new AnonymousSemantics(pow10, {TINT}, TINT, "pow10");
        auto c = defaultExampleConfig();
        std::vector<ExternalExtraSemantics> extra_list;
        extra_list.emplace_back(semantics::getSemanticsFromName("*"));
        if (!benchmark::KIsSinglePass) extra_list.emplace_back(pow10_info);
        else {
            extra_list.emplace_back(new ConstSemantics(10));
        }
        c.int_min = 0;
        c.int_max = 9;
        c.max_length = 4;
        return benchmark::defaultBuilder(tactic_name, "atoi", atoi_semantics, c, {}, extra_list);
    }

    Task* dropWhileBuilder() {
        auto drop_while_semantics = [](DataList&& inp, GlobalInfo* info) {
            auto list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                if (list[i]) return i;
            }
            return 0;
        };
        auto c = defaultExampleConfig();
        c.int_min = 0; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "dropwhlie", drop_while_semantics, c);
    }

    Task* balancedBuilder() {
        auto balanced_semantics = [](DataList&& inp, GlobalInfo* info) {
            int cnt = 0;
            auto list = inp[0].moveList();
            for (int v: list) {
                cnt += v;
                if (cnt < 0) return 0;
            }
            return 1;
        };
        auto c = defaultExampleConfig();
        c.int_min = -1; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "balanced", balanced_semantics, c);
    }

    Task* zeroStartOneStarBuilder() {
        auto zero_star_one_star_semantics = [](DataList&& inp, GlobalInfo* info) {
            int an = 1, bn = 1;
            auto list = inp[0].moveList();
            for (auto v: list) {
                an = v && an;
                bn = ((!v) || an) && bn;
            }
            return bn;
        };
        auto c = defaultExampleConfig();
        c.int_min = 0; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "0*1*", zero_star_one_star_semantics, c);
    }

    Task* cntOnesBuilder() {
        auto cnt_ones_semantics = [](DataList&& inp, GlobalInfo* info) {
            int i = 0, f = 0, cnt = 0;
            auto a = inp[0].moveList(); int n = a.size();
            for(i =0; i < n; i++) {
                cnt += (a[i] && !f) ? 1 : 0;
                f = a[i];
            }
            return cnt;
        };
        auto c = defaultExampleConfig();
        c.int_min = 0; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "cnt_1s", cnt_ones_semantics, c);
    }

    Task* lineSightBuilder() {
        auto line_sight_semantics = [](DataList&& inp, GlobalInfo* info) {
            int ma = 0, vis = 1;
            auto list = inp[0].moveList();
            for (auto v: list) {
                vis = (ma <= v);
                ma = std::max(ma, v);
            }
            return vis;
        };
        return benchmark::defaultBuilder(tactic_name, "line_sight", line_sight_semantics, defaultExampleConfig());
    }

    Task* maxLenOnesBuilder() {
        auto max_len_1s_semantics = [](DataList&& inp, GlobalInfo* info) {
            int ma = 0, l = 0;
            auto list = inp[0].getList();
            for (int i = 0; i <= list.size(); ++i) {
                if (i == list.size() || list[i] == 0) {
                    ma = std::max(ma, l);
                    l = 0;
                }  else ++l;
            }
            return ma;
        };
        auto c = defaultExampleConfig();
        c.int_min = 0; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "max_len_1s", max_len_1s_semantics, c);
    }

    Task* zeroAfterOneBuilder() {
        auto zero_after_1_semantics = [](DataList&& inp, GlobalInfo* info) -> Data {
            bool seen1 = false, res = false;
            auto l(inp[0].getList());
            for (int i = 0; i < l.size(); ++i) {
                if (seen1 && !(l[i])) res = true;
                seen1 = seen1 || l[i];
            }
            return new IntValue(res);
        };
        auto c = defaultExampleConfig();
        c.int_min = 0; c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "0after1", zero_after_1_semantics, c);
    }
}

#include "enumerator.h"

namespace regex_tool {

    std::vector<std::string> getRegList(const std::vector<std::string>& value_list, int char_limit, int num_limit) {
        std::vector<std::string> reg_list[10][20], pure[10][20];
        std::vector<std::string> reg;
        for (std::string r: value_list) {
            reg.push_back(r);
            pure[0][1].push_back(r);
            if (r[r.size() - 1] != '+' && r[r.size() - 1] != '*') {
                reg.push_back(r + "+");
                pure[1][1].push_back(r + "+");
                reg.push_back(r + "*");
                pure[1][1].push_back(r + "*");
            }
        }
        if (reg.size() > num_limit) return reg;
        for (int size = 1;; ++size) {
            for (int char_num = 1; char_num <= char_limit; ++char_num) {
                for (int l_size = 0; l_size <= size - 1; ++l_size)
                    for (int l_num = 1; l_num < char_num; ++l_num) {
                        int r_size = size - l_size - 1;
                        int r_num = char_num - l_num;
                        std::vector<std::string> A;
                        for (auto lr: reg_list[l_size][l_num]) for (auto rr: pure[r_size][r_num]) {
                                A.push_back(lr + rr);
                            }
                        for (auto lr: pure[l_size][l_num]) for (auto rr: reg_list[r_size][r_num]) {
                                A.push_back(lr + rr);
                            }
                        for (auto lr: pure[l_size][l_num]) for (auto rr: pure[r_size][r_num]) {
                                A.push_back(lr + rr);
                            }
                        for (auto r: A) {
                            reg.push_back(r);
                            reg_list[size][char_num].push_back(r);
                            if (reg.size() == num_limit) return reg;
                        }
                    }
                for (auto r: reg_list[size - 1][char_num]) {
                    auto r_new = "(" + r + ")*";
                    reg.push_back(r_new);
                    pure[size][char_num].push_back(r_new);
                    if (reg.size() == num_limit) return reg;
                    r_new = "(" + r + ")+";
                    reg.push_back(r_new);
                    pure[size][char_num].push_back(r_new);
                    if (reg.size() == num_limit) return reg;
                }
            }
        }
    }

    std::vector<ExtraSemantics> extraSemanticsForReg(const std::vector<std::string>& value_list, int char_limit, int num_limit) {
        auto r_list = getRegList(value_list, char_limit, num_limit);
        std::vector<ExtraSemantics> result;
        for (const auto& r: r_list) {
            auto aux = [r](DataList&& inp, GlobalInfo* info) {
                LIST list = inp[0].moveList();
                std::regex re(r);
                int num = 0;
                std::string s;
                for (auto& v: list) {
                    if (0 <= v && v <= 9) s += '0' + v; else s += '?';
                }
                std::regex_iterator<std::string::const_iterator> it(s.begin(), s.end(), re);
                std::vector<int> result;
                if (it == std::sregex_iterator() || it->position(0) != 0) return new ListValue(result);
                for (int i = 0; i < it->length(); ++i) {
                    result.push_back(list[i]);
                }
                return new ListValue(result);
            };
            result.emplace_back(new AnonymousSemantics(aux, {TLIST}, TLIST, "prefix_" + r));
            auto aux_suf = [r](DataList&& inp, GlobalInfo* info) {
                LIST list = inp[0].moveList();
                std::regex re(r);
                int num = 0;
                std::string s;
                for (auto& v: list) {
                    if (0 <= v && v <= 9) s += '0' + v; else s += '?';
                }
                std::regex_iterator<std::string::const_iterator> it(s.begin(), s.end(), re);
                std::vector<int> result;
                while (it != std::sregex_iterator()) {
                    int l = it->position(0);
                    int r = l + it->length();
                    if (r == list.size()) {
                        for (int i = l; i < r; ++i) {
                            result.push_back(list[i]);
                        }
                        break;
                    }
                    ++it;
                }
                return new ListValue(result);
            };
            result.emplace_back(new AnonymousSemantics(aux_suf, {TLIST}, TLIST, "suffix_" + r));
        }
        return result;
    }
}

namespace pldi21 {
    Task* count10PlusBuilder() {
        // Such a implementation ignores the last 1(0+)
        auto count_10plus_semantics = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            bool s0 = false, s1 = false;
            int result = 0;
            LIST list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                result += (s1 && (list[i] != 0)) ? 1 : 0;
                s1 = (list[i] == 0) && (s0 || s1);
                s0 = list[i] == 1;
            }
            return result;
        };
        auto s1_semantics = [](DataList&& inp, GlobalInfo* global_info) {
            bool s0 = false, s1 = false;
            LIST list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                s1 = (list[i] == 0) && (s0 || s1);
                s0 = list[i] = 1;
            }
            return int(s1);
        };
        auto s1 = new AnonymousSemantics(s1_semantics, {TLIST}, TINT, "s1");
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 1;
        return benchmark::defaultBuilder(tactic_name, "count1(0+)", count_10plus_semantics, c);
    }

    Task* count10star2Builder() {
        auto count_10star2_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
            bool s0 = false;
            int c = 0;
            LIST list = inp[0].getList();
            for (int i = 0; i < list.size(); ++i) {
                c += s0 && (list[i] == 2 ? 1 : 0);
                s0 = (list[i] == 1) || (s0 && list[i] == 0);
            }
            return c;
        };
        std::vector<ExtraSemantics> extra;
        if (!benchmark::KIsSinglePass) extra = regex_tool::extraSemanticsForReg({"0", "1", "2"}, 3, 50);
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 5;
        return benchmark::defaultBuilder(tactic_name, "count1(0*)2", count_10star2_semantics, c, extra);
    }

    Task* count1star2star3starBuilder() {
        // Such an implementation actually count 1+2*3+
        auto count_1star2star3star_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
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
        // regex_util::buildNonExtraSemanticsForRE({1, 2, 3});
        auto c = defaultExampleConfig();
        c.int_min = 1;
        c.int_max = 3;
        return benchmark::defaultBuilder(tactic_name, "count1*2*3*", count_1star2star3star_semantics, c);
    }

    Task* maxSumBetweenOnesBuilder() {
        auto max_sum_between_ones_semantics = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            int ms = 0, cs = 0;
            LIST list = inp[0].moveList();
            for (int i = 0; i < list.size(); ++i) {
                cs = list[i] != 1 ? cs + list[i] : 0;
                ms = std::max(ms, cs);
            }
            return ms;
        };
        auto prefix_till_one = [](DataList&& inp, GlobalInfo* global_info) -> Data{
            auto list(inp[0].moveList());
            LIST result;
            for (int i = 0; i < list.size(); ++i) {
                if (list[i] != 1) {
                    result.push_back(list[i]);
                } else break;
            }
            return new ListValue(result);
        };
        auto prefix_till_one_semantics = new AnonymousSemantics(prefix_till_one, {TLIST}, TLIST, "prefix_till_1");
        std::vector<ExtraSemantics> extra_list;
        extra_list.emplace_back(prefix_till_one_semantics);
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 3;
        return benchmark::defaultBuilder(tactic_name, "max_sum_between_ones", max_sum_between_ones_semantics, c, extra_list);
    }

    Task* maxDistanceBetweenZerosBuilder() {
        auto max_distance_between_zeros_semantics = [](DataList&& inp, GlobalInfo* global_info) -> Data {
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
        return benchmark::defaultBuilder(tactic_name, "max_dist_between_zeros", max_distance_between_zeros_semantics, c);
    }

    Task* lisBuilder() {
        auto lis_semantics = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            LIST list = inp[0].getList();
            int cl = 0, ml = 0, prev = list[0];
            for (int i = 1; i < list.size(); ++i) {
                cl = prev < list[i] ? cl + 1 : 0;
                ml = std::max(ml, cl);
                prev = list[i];
            }
            return ml;
        };

        /* extra semantics*/
        auto longest_prefix = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            LIST list = inp[0].getList();

            auto* semantics = dynamic_cast<SemanticsValue*>(inp[1].value);
            auto* f = semantics->semantics;
            int res = 1;
            while (res < list.size() && f->run({Data(new IntValue(list[res - 1])), Data(new IntValue(list[res]))}, nullptr).getBool()) ++res;
            return res;
        };
        auto* nt_int = new NonTerminal(util::getDefaultSymbolForType(TINT), TINT);
        auto* bool_bif = new NonTerminal("bool_bif", TSEMANTICS);
        auto* nt_list = new NonTerminal(util::getDefaultSymbolForType(TLIST), TLIST);
        nt_int->rule_list.push_back(new Rule(new AnonymousSemantics(longest_prefix, {TLIST, TSEMANTICS}, TINT, "longest_prefix"),
                                             {nt_list, bool_bif}));
        bool_bif->rule_list.push_back(new Rule(semantics::getSemanticsFromName("<"), {}));
        bool_bif->rule_list.push_back(new Rule(semantics::getSemanticsFromName(">"), {}));

        std::vector<ExtraSemantics> extra_list;
        extra_list.emplace_back((std::vector<NonTerminal*>){nt_int, bool_bif, nt_list});

        return benchmark::defaultBuilder(tactic_name, "lis", lis_semantics, defaultExampleConfig(), extra_list);
    }

    Task* largestPeakBuilder() {
        auto largest_peak_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
            LIST list = inp[0].getList();
            int cmo = 0, lpeak = 0;
            for (int i = 0; i < list.size(); ++i) {
                cmo = list[i] > 0 ? cmo + list[i] : 0;
                lpeak = std::max(cmo, lpeak);
            }
            return lpeak;
        };

        /* extra info*/
        auto longest_prefix = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            LIST list = inp[0].getList();
            LIST p;
            auto* semantics = dynamic_cast<SemanticsValue*>(inp[1].value);
            auto* f = semantics->semantics;
            int res = 0, sum = 0;
            while (res < list.size() && f->run({Data(new IntValue(list[res]))}, nullptr).getBool()) {
                p.push_back(list[res]); ++res;
            }
            return new ListValue(p);
        };
        auto* bool_hf = new NonTerminal("bool_hf", TSEMANTICS);
        auto* nt_list = new NonTerminal(util::getDefaultSymbolForType(TLIST), TLIST);
        nt_list->rule_list.push_back(new Rule(new AnonymousSemantics(longest_prefix, {TLIST, TSEMANTICS}, TLIST, "longest_prefix"),
                                              {nt_list, bool_hf}));
        std::vector<ExtraSemantics> extra;
        extra.emplace_back((std::vector<NonTerminal*>){bool_hf, nt_list});
        return benchmark::defaultBuilder(tactic_name, "largest_peak", largest_peak_semantics, defaultExampleConfig(),
                                         extra);
    }

    Task* longest1starBuilder() {
        auto longest_1star_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
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
        return benchmark::defaultBuilder(tactic_name, "longest1*", longest_1star_semantics, c);
    }

    Task* longest10star2Builder() {
        auto longest_10star2_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
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
        std::vector<ExtraSemantics> extra;
        if (!benchmark::KIsSinglePass) extra = regex_tool::extraSemanticsForReg({"0", "1", "2"}, 3, 50);
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 2;
        return benchmark::defaultBuilder(tactic_name, "longest1(0*)2", longest_10star2_semantics, c, extra);
    }

    Task* longestEven0starBuilder() {
        auto longest_even_0star_semantics = [](DataList &&inp, GlobalInfo* global_info) -> Data {
            int cl = 0, ml = 0, ml_tmp = 0;
            LIST list = inp[0].getList();
            for (int i = 0; i < list.size(); ++i) {
                cl = list[i] == 0 ? cl + 1 : 0;
                ml_tmp = std::max(ml_tmp, cl);
                if (ml_tmp % 2 == 0) ml = ml_tmp;
            }
            return ml;
        };
        auto is_even = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            return inp[0].getInt() % 2;
        };
        ExternalExtraSemantics is_even_info(new AnonymousSemantics(is_even, {TINT}, TINT, "is_even"));
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 1;
        config::KMaxBranchNum = 5;
        return benchmark::defaultBuilder(tactic_name, "longest(00)*", longest_even_0star_semantics, c, {}, {is_even_info});
    }

    Task* longestOdd10starBuilder() {
        auto longest_odd_10star = [](DataList &&inp, GlobalInfo* global_info) -> Data {
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
        std::vector<ExtraSemantics> ex_list;
        ex_list = regex_tool::extraSemanticsForReg({"0*", "1", "(0+1)*"}, 3, 50);
        auto is_even = [](DataList&& inp, GlobalInfo* global_info) -> Data {
            return inp[0].getInt() % 2;
        };
        ExternalExtraSemantics is_even_info(new AnonymousSemantics(is_even, {TINT}, TINT, "is_even"), true);
        auto c = defaultExampleConfig();
        c.int_min = 0;
        c.int_max = 1;
        c.max_length = 15;
        return benchmark::defaultBuilder(tactic_name, "longest_odd_(10*)", longest_odd_10star, c,
                                         ex_list, {is_even_info});
    }
}

namespace extra {
    Task* thirdMinBuilder() {
        auto third_min_semantics = [](DataList&& inp, GlobalInfo* info) {
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
        return benchmark::defaultBuilder(tactic_name, "3rd-min", third_min_semantics, defaultExampleConfig());
    }

    Task* mppBuilder() {
        auto mpp_semantics = [](DataList &&inp, GlobalInfo* global_info) {
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
        return benchmark::defaultBuilder(tactic_name, "mpp", mpp_semantics, c, {}, {semantics::getSemanticsFromName("*")});
    }

    Task* mtpBuilder() {
        auto mtp_semantics = [](DataList &&inp, GlobalInfo* global_info) {
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
        return benchmark::defaultBuilder(tactic_name, "mtp", mtp_semantics, c, {}, {semantics::getSemanticsFromName("*")});
    }

    Task* mspBuilder() {
        auto msp_semantics = [](DataList &&inp, GlobalInfo* global_info) {
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
        return benchmark::defaultBuilder(tactic_name, "msp", msp_semantics, c, {}, {semantics::getSemanticsFromName("*")});
    }

    Task* max1sPBuilder() {
        auto max_1s_semantics = [](DataList&& inp, GlobalInfo* global_info)->Data {
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
        auto max_1s_p_semantics = [](DataList&& inp, GlobalInfo* global_info) -> Data {
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
        return benchmark::defaultBuilder(tactic_name, "max_1s_p", max_1s_p_semantics, c,
                                         {new AnonymousSemantics(max_1s_semantics, {TLIST}, TINT, "max_1s")});
    }
}

namespace oopsla {
    Task* misBuilder() {
        auto mis_semantics = [](DataList &&inp, GlobalInfo* global_info) {
            auto list(inp[0].moveList());
            int p = 0, np = 0;
            for (int v: list) {
                int prep = p;
                p = np + v;
                np = std::max(prep, np);
            }
            return std::max(p, np);
        };
        return benchmark::defaultBuilder(tactic_name, "mis", mis_semantics, defaultExampleConfig(),
                                         {new AnonymousSemantics(mis_semantics, {TLIST}, TINT, "mis")});
    }

    Task* masBuilder() {
        auto mis_semantics = [](DataList &&inp, GlobalInfo* global_info) {
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
        return benchmark::defaultBuilder(tactic_name, "mis", mis_semantics, defaultExampleConfig());
    }

    Task* mmmBuilder() {
        auto mmm_semantics = [](DataList &&inp, GlobalInfo* global_info) {
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
        return benchmark::defaultBuilder(tactic_name, "mmm", mmm_semantics, defaultExampleConfig(),
                                         {new AnonymousSemantics(mmm_semantics, {TLIST}, TINT, "mmm")});
    }
}

namespace {
    std::map<std::string, TaskBuilder> benchmark_map = {
            {"sum", pldi17::sumBuilder}, {"min", pldi17::minBuilder}, {"max", pldi17::maxBuilder},
            {"length", pldi17::lengthBuilder}, {"2nd-min", pldi17::secondMinBuilder},
            {"mps", pldi17::mpsBuilder}, {"mts", pldi17::mtsBuilder}, {"mss", pldi17::mssBuilder},
            {"mps_p", pldi17::mpsPosBuilder}, {"mts_p", pldi17::mtsPosBuilder}, {"is_sorted", pldi17::isSortedBuilder},
            {"atoi", pldi17::atoiBuilder},  {"dropwhile", pldi17::dropWhileBuilder}, {"balanced", pldi17::balancedBuilder},
            {"0*1*", pldi17::zeroStartOneStarBuilder}, {"cnt_1s", pldi17::cntOnesBuilder}, {"0after1", pldi17::zeroAfterOneBuilder},
            {"line_sight", pldi17::lineSightBuilder}, {"max_len_1s", pldi17::maxLenOnesBuilder},
            {"average", pldi17::averageBuilder}, {"count1(0+)", pldi21::count10PlusBuilder},
            {"count1(0*)2", pldi21::count10star2Builder}, {"count1*2*3*", pldi21::count1star2star3starBuilder},
            {"max_sum_between_ones", pldi21::maxSumBetweenOnesBuilder}, {"max_dist_between_zeros", pldi21::maxDistanceBetweenZerosBuilder},
            {"lis", pldi21::lisBuilder}, {"largest_peak", pldi21::largestPeakBuilder},
            {"longest1*", pldi21::longest1starBuilder}, {"longest1(0*)2", pldi21::longest10star2Builder},
            {"longest(00)*", pldi21::longestEven0starBuilder}, {"longest_odd(10)*", pldi21::longestOdd10starBuilder},
            {"3rd-min", extra::thirdMinBuilder}, {"mpp", extra::mppBuilder},
            {"mtp", extra::mtpBuilder}, {"msp", extra::mspBuilder},
            {"max_1s_p", extra::max1sPBuilder}, {"mis", oopsla::misBuilder},
            {"mas", oopsla::masBuilder}, {"mmm", oopsla::mmmBuilder}
    };
}

Task * benchmark::getDivideAndConquer(const std::string& name) {
    tactic_name = "tree";
    return benchmark_map[name]();
}

Task * benchmark::getListR(const std::string &name) {
    tactic_name = "listr";
    return benchmark_map[name]();
}

