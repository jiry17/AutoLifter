//
// Created by jiruy on 2020/11/21.
//

#ifndef CPP_SEMANTICS_FACTORY_H
#define CPP_SEMANTICS_FACTORY_H

#include "semantics.h"
#include "config.h"

namespace {
    int roundIntoRange(long long w) {
        if (w > config::KExampleIntMax) return config::KExampleIntMax;
        else if (w < -config::KExampleIntMax) return -config::KExampleIntMax;
        return w;
    }
}

class LinkSemantics: public Semantics {
public:
    LinkSemantics(): Semantics({TLIST, TLIST}, TLIST, "++") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> l_list(inp[0].moveList()), r_list(inp[1].moveList());
        for (auto v: r_list) l_list.push_back(v);
        return new ListValue(std::move(l_list));
    }
};

class HeadSemantics: public Semantics {
public:
    HeadSemantics(): Semantics({TLIST}, TINT, "head") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return inp[0].getRDefault();
        return new IntValue(list[0]);
    }
};

class LastSemantics: public Semantics {
public:
    LastSemantics(): Semantics({TLIST}, TINT, "last") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return inp[0].getLDefault();
        return new IntValue(list[list.size() - 1]);
    }
};

class TakeSemantics: public Semantics {
public:
    TakeSemantics(): Semantics({TINT, TLIST}, TINT, "take") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        std::vector<int> result;
        for (int i = 0; i < n && i < pos; ++i) {
            result.push_back(list[i]);
        }
        return new ListValue(std::move(result));
    }
};

class DropSemantics: public Semantics {
public:
    DropSemantics(): Semantics({TINT, TLIST}, TINT, "drop") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        pos = std::max(pos, 0);
        std::vector<int> result;
        for (int i = pos; i < n; ++i) {
            result.push_back(list[i]);
        }
        return new ListValue(std::move(result));
    }
};

class AccessSemantics: public Semantics {
public:
    AccessSemantics(): Semantics({TINT, TLIST}, TINT, "access") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        int pos = inp[0].getInt(), n = (int)list.size();
        if (pos < 0) pos += n;
        if (pos < 0) return inp[1].getLDefault();
        if (pos >= n) return inp[1].getRDefault();
        return new IntValue(list[pos]);
    }
};

class MinimumSemantics: public Semantics {
public:
    MinimumSemantics(): Semantics({TLIST}, TINT, "minimum") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return config::KDefaultValue;
        int result = list[0];
        for (int i = 1; i < list.size(); ++i) {
            result = std::min(result, list[i]);
        }
        return new IntValue(result);
    }
};

class MaximumSemantics: public Semantics {
public:
    MaximumSemantics(): Semantics({TLIST}, TINT, "maximum") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        if (list.empty()) return -config::KDefaultValue;
        int result = list[0];
        for (int i = 1; i < list.size(); ++i) {
            result = std::max(result, list[i]);
        }
        return new IntValue(result);
    }
};

class ReverseSemantics: public Semantics {
public:
    ReverseSemantics(): Semantics({TLIST}, TLIST, "rev") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        std::reverse(list.begin(), list.end());
        return new ListValue(std::move(list));
    }
};

class SortSemantics: public Semantics {
public:
    SortSemantics(): Semantics({TLIST}, TLIST, "sort") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        std::sort(list.begin(), list.end());
        return new ListValue(std::move(list), -config::KDefaultValue, config::KDefaultValue);
    }
};

class SumSemantics: public Semantics {
public:
    SumSemantics(): Semantics({TLIST}, TINT, "sum") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[0].moveList());
        int sum = 0;
        for (int i = 0; i < list.size(); ++i) {
            sum += list[i];
        }
        return sum;
    }
};

class MapSemantics: public Semantics {
public:
    MapSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "map") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        for (int i = 0; i < list.size(); ++i) {
            list[i] = f->run({Data(new IntValue(list[i]))}, global_info).getInt();
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class FilterSemantics: public Semantics {
public:
    FilterSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "filter") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        std::vector<int> result;
        for (int i = 0; i < list.size(); ++i) {
            if (f->run({Data(new IntValue(list[i]))}, global_info).getBool()) {
                result.push_back(list[i]);
            }
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class CountSemantics: public Semantics {
public:
    CountSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "count") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        int result = 0;
        for (int i = 0; i < list.size(); ++i) {
            auto w = f->run({Data(new IntValue(list[i]))}, global_info);
            if (w.getBool()) {
                result += 1;
            }
        }
        semantics->releaseSemantics();
        return new IntValue(result);
    }
};

class ZipwithSemantics: public Semantics {
public:
    ZipwithSemantics(): Semantics({TSEMANTICS, TLIST, TLIST}, TLIST, "zipwith") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> l(inp[1].moveList()), r(inp[2].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        std::vector<int> result;
        for (int i = 0; i < l.size() && i < r.size(); ++i) {
            result.push_back(f->run({Data(new IntValue(l[i])), Data(new IntValue(r[i]))}, global_info).getInt());
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class ScanlSemantics: public Semantics {
public:
    ScanlSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanl") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        if (list.empty()) return new ListValue(std::move(list));
        int pre = list[0];
        for (int i = 1; i < list.size(); ++i) {
            pre = f->run({Data(new IntValue(pre)), Data(new IntValue(list[i]))}, global_info).getInt();
            list[i] = pre;
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class ScanrSemantics: public Semantics {
public:
    ScanrSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanr") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        if (list.empty()) return Data(new ListValue({}));
        std::reverse(list.begin(), list.end());
        int pre = list[0];
        for (int i = 1; i < list.size(); ++i) {
            pre = f->run({Data(new IntValue(pre)), Data(new IntValue(list[i]))}, global_info).getInt();
            list[i] = pre;
        }
        std::reverse(list.begin(), list.end());
        semantics->releaseSemantics();
        return new ListValue(std::move(list));
    }
};

class PlusSemantics: public Semantics {
public:
    PlusSemantics(): Semantics({TINT, TINT}, TINT, "+") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].isError()) return inp[0];
        if (inp[1].isError()) return inp[1];
        int l = inp[0].getInt(), r = inp[1].getInt();
        return roundIntoRange(l + r);
    }
};

class MinusSemantics: public Semantics {
public:
    MinusSemantics(): Semantics({TINT, TINT}, TINT, "-") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].isError()) return inp[0];
        if (inp[1].isError()) return inp[1];
        int l = inp[0].getInt(), r = inp[1].getInt();
        return roundIntoRange(l - r);
    }
};

class TimeSemantics: public Semantics {
public:
    TimeSemantics(): Semantics({TINT, TINT}, TINT, "*") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].isError()) return inp[0];
        if (inp[1].isError()) return inp[1];
        int l = inp[0].getInt(), r = inp[1].getInt();
        return roundIntoRange(1ll * l * r);
    }
};

class MinSemantics: public Semantics {
public:
    MinSemantics(): Semantics({TINT, TINT}, TINT, "min") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new IntValue(std::min(l, r));
    }
};

class MaxSemantics: public Semantics {
public:
    MaxSemantics(): Semantics({TINT, TINT}, TINT, "max") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new IntValue(std::max(l, r));
    }
};

class SquareSemantics: public Semantics {
public:
    SquareSemantics(): Semantics({TINT}, TINT, "square") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].isError()) return inp[0];
        int l = inp[0].getInt();
        return new IntValue(l * l);
    }
};

class LqSemantics: public Semantics {
public:
    LqSemantics(): Semantics({TINT, TINT}, TBOOL, "<") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new BoolValue(l < r);
    }
};

class GqSemantics: public Semantics {
public:
    GqSemantics(): Semantics({TINT, TINT}, TBOOL, ">") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt(), r = inp[1].getInt();
        return new BoolValue(l > r);
    }
};

class EvenSemantics: public Semantics {
public:
    EvenSemantics(): Semantics({TINT}, TBOOL, "even") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt();
        return new BoolValue(l % 2 == 0);
    }
};

class OddSemantics: public Semantics {
public:
    OddSemantics(): Semantics({TINT}, TBOOL, "odd") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int l = inp[0].getInt();
        return Data(new BoolValue(l % 2 == 1));
    }
};

class LengthSemantics: public Semantics {
public:
    LengthSemantics(): Semantics({TLIST}, TINT, "len") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> list = inp[0].getList();
        return new IntValue(list.size());
    }
};

class ApplyTwoSemantics: public Semantics {
public:
    ApplyTwoSemantics(): Semantics({TSEMANTICS, TINT, TINT}, TINT, "apply2") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        auto result = f->run({std::move(inp[1]), std::move(inp[2])}, global_info);
        semantics->releaseSemantics();
        return std::move(result);
    }
};

class LtZeroSemantics: public Semantics {
public:
    LtZeroSemantics(): Semantics({TINT}, TBOOL, "lt_zero") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt();
        return new BoolValue(x < 0);
    }
};

class GtZeroSemantics: public Semantics {
public:
    GtZeroSemantics(): Semantics({TINT}, TBOOL, "gt_zero") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt();
        return new BoolValue(x > 0 );
    }
};

class LeqSemantics: public Semantics {
public:
    LeqSemantics(): Semantics({TINT, TINT}, TBOOL, "<=") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int x = inp[0].getInt(), y = inp[1].getInt();
        return new BoolValue(x <= y);
    }
};

class IteIntSemantics: public Semantics {
public:
    IteIntSemantics(): Semantics({TBOOL, TINT, TINT}, TINT, "ite") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        if (inp[0].getBool()) return std::move(inp[1]); else return std::move(inp[2]);
    }
};

class EqSemantics: public Semantics {
public:
    EqSemantics(): Semantics({TINT, TINT}, TBOOL, "=") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return inp[0].getInt() == inp[1].getInt();
    }
};

class NeqSemantics: public Semantics {
public:
    NeqSemantics(): Semantics({TINT, TINT}, TBOOL, "!=") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return inp[0].getInt() != inp[1].getInt();
    }
};

class FilterIndexSemantics: public Semantics {
public:
    FilterIndexSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "filter_index") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        std::vector<int> result;
        auto list(inp[1].moveList());
        auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
        auto* f = semantics->semantics;
        for (int i = 0; i < list.size(); ++i) {
            if (f->run({Data(new IntValue(list[i]))}, global_info).getBool()) {
                result.push_back(i);
            }
        }
        semantics->releaseSemantics();
        return new ListValue(std::move(result));
    }
};

class NotSemantics: public Semantics {
public:
    NotSemantics(): Semantics({TBOOL}, TBOOL, "not") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(!inp[0].getBool());
    }
};

class AndSemantics: public Semantics {
public:
    AndSemantics(): Semantics({TBOOL, TBOOL}, TBOOL, "and") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(inp[0].getBool() & inp[1].getBool());
    }
};

class PowSemantics: public Semantics {
public:
    PowSemantics(): Semantics({TINT, TINT}, TINT, "pow") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        int a = inp[0].getInt(), b = inp[1].getInt();
        if (a < 0 || b < 0) return new NoneValue();
        long long ans = 1;
        for (int i = 1; i <= b; ++i) {
            ans *= a;
            if (ans >= 1e9) return new NoneValue();
        }
        return int(ans);
    }
};

class OrSemantics: public Semantics {
public:
    OrSemantics(): Semantics({TBOOL, TBOOL}, TBOOL, "or") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new BoolValue(inp[0].getBool() | inp[1].getBool());
    }
};

class NegSemantics: public Semantics {
public:
    NegSemantics(): Semantics({TINT}, TINT, "neg") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return -inp[0].getInt();
    }
};

class SingleListSemantics: public Semantics {
public:
    SingleListSemantics(): Semantics({TINT}, TLIST, "[]") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return new ListValue({inp[0].getInt()});
    }
};

class ConsSemantics: public Semantics {
public:
    ConsSemantics(): Semantics({TINT, TLIST}, TLIST, "cons") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        LIST result = {inp[0].getInt()};
        for (auto& w: inp[1].moveList()) {
            result.push_back(w);
        }
        return new ListValue(std::move(result));
    }
};

class IntPassSemantics: public Semantics {
public:
    IntPassSemantics(): Semantics({TINT}, TINT, "int") {}
    virtual Data run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        return inp[0];
    }
};

class AppendSemantics: public Semantics {
public:
    AppendSemantics(): Semantics({TLIST, TINT}, TLIST, "append") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        LIST x = inp[0].moveList(); int y = inp[1].getInt();
        x.push_back(y);
        return new ListValue(std::move(x));
    }
};

class TailSemantics: public Semantics {
public:
    TailSemantics(): Semantics({TLIST}, TLIST, "tail") {}
    virtual Data run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
        check(inp);
#endif
        LIST x = inp[0].moveList();
        LIST result;
        for (int i = 1; i < x.size(); ++i) {
            result.push_back(x[i]);
        }
        return new ListValue(std::move(result));
    }
};

namespace semantics {
    Semantics* getSemanticsFromName(std::string name);
}

#endif //CPP_SEMANTICS_FACTORY_H
