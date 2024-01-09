//
// Created by jiruy on 2020/11/21.
//

#include "basic/semantics_factory.h"
#include <map>
#include <iostream>

namespace {
    std::map<std::string, Semantics*> semantics_map = {
            {"take", new TakeSemantics()}, {"drop", new DropSemantics()},
            {"rev", new ReverseSemantics()}, {"sort", new SortSemantics()},
            {"filter", new FilterSemantics()}, {"zipwith", new ZipwithSemantics()},
            {"scanl", new ScanlSemantics()}, {"scanr", new ScanrSemantics()},
            {"map", new MapSemantics()}, {"head", new HeadSemantics()},
            {"last", new LastSemantics()}, {"access", new AccessSemantics()},
            {"minimum", new MinimumSemantics()}, {"maximum", new MaximumSemantics()},
            {"sum", new SumSemantics()}, {"length", new LengthSemantics()},
            {"count", new CountSemantics()}, {"apply2", new ApplyTwoSemantics()},
            {"+", new PlusSemantics()}, {"-", new MinusSemantics()},
            {"lt_zero", new LtZeroSemantics()}, {"gt_zero", new GtZeroSemantics()},
            {"odd", new OddSemantics()}, {"even", new EvenSemantics()},
            {"min", new MinSemantics()}, {"max", new MaxSemantics()},
            {"cons", new ConsSemantics()}, {"++", new LinkSemantics()},
            {"<", new LqSemantics()}, {"<=", new LeqSemantics()},
            {">", new GqSemantics()},
            {"ite", new IteIntSemantics()}, {"=", new EqSemantics()},
            {"not", new NotSemantics()},
            {"and", new AndSemantics()}, {"or", new OrSemantics()},
            {"*", new TimeSemantics()},
            {"neg", new NegSemantics()}, {"[]", new SingleListSemantics()},
            {"cons", new ConsSemantics()}, {"int", new IntPassSemantics()},
            {"append", new AppendSemantics()}, {"tail", new TailSemantics()},
            {"!=", new NeqSemantics()}
    };
}

Semantics * semantics::getSemanticsFromName(std::string name) {
    assert(semantics_map.count(name));
    return semantics_map[name];
}


namespace {
    int roundIntoRange(long long w) {
        if (w > config::KExampleIntMax) return config::KExampleIntMax;
        else if (w < -config::KExampleIntMax) return -config::KExampleIntMax;
        return w;
    }
}


LinkSemantics::LinkSemantics(): Semantics({TLIST, TLIST}, TLIST, "++") {}

Data LinkSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> l_list(inp[0].moveList()), r_list(inp[1].moveList());
    for (auto v: r_list) l_list.push_back(v);
    return new ListValue(std::move(l_list));
}


HeadSemantics::HeadSemantics(): Semantics({TLIST}, TINT, "head") {}

Data HeadSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> list(inp[0].moveList());
    if (list.empty()) return inp[0].getRDefault();
    return new IntValue(list[0]);
}


LastSemantics::LastSemantics(): Semantics({TLIST}, TINT, "last") {}
Data LastSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> list(inp[0].moveList());
    if (list.empty()) return inp[0].getLDefault();
    return new IntValue(list[list.size() - 1]);
}

TakeSemantics::TakeSemantics(): Semantics({TINT, TLIST}, TINT, "take") {}
Data TakeSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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

DropSemantics::DropSemantics(): Semantics({TINT, TLIST}, TINT, "drop") {}
Data DropSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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

AccessSemantics::AccessSemantics(): Semantics({TINT, TLIST}, TINT, "access") {}
Data AccessSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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


MinimumSemantics::MinimumSemantics(): Semantics({TLIST}, TINT, "minimum") {}
Data MinimumSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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


MaximumSemantics::MaximumSemantics(): Semantics({TLIST}, TINT, "maximum") {}
Data MaximumSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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

ReverseSemantics::ReverseSemantics(): Semantics({TLIST}, TLIST, "rev") {}
Data ReverseSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> list(inp[0].moveList());
    std::reverse(list.begin(), list.end());
    return new ListValue(std::move(list));
}


SortSemantics::SortSemantics(): Semantics({TLIST}, TLIST, "sort") {}
Data SortSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> list(inp[0].moveList());
    std::sort(list.begin(), list.end());
    return new ListValue(std::move(list), -config::KDefaultValue, config::KDefaultValue);
}


SumSemantics::SumSemantics(): Semantics({TLIST}, TINT, "sum") {}
Data SumSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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

MapSemantics::MapSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "map") {}
Data MapSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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

FilterSemantics::FilterSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "filter") {}
Data FilterSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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


CountSemantics::CountSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "count") {}
Data CountSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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

ZipwithSemantics::ZipwithSemantics(): Semantics({TSEMANTICS, TLIST, TLIST}, TLIST, "zipwith") {}
Data ZipwithSemantics::run(DataList&& inp, GlobalInfo* global_info) {
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

ScanlSemantics::ScanlSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanl") {}
Data ScanlSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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

ScanrSemantics::ScanrSemantics(): Semantics({TSEMANTICS, TLIST}, TLIST, "scanr") {}
Data ScanrSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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

PlusSemantics::PlusSemantics(): Semantics({TINT, TINT}, TINT, "+") {}
Data PlusSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    if (inp[0].isError()) return inp[0];
    if (inp[1].isError()) return inp[1];
    int l = inp[0].getInt(), r = inp[1].getInt();
    return roundIntoRange(l + r);
}


MinusSemantics::MinusSemantics(): Semantics({TINT, TINT}, TINT, "-") {}
Data MinusSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    if (inp[0].isError()) return inp[0];
    if (inp[1].isError()) return inp[1];
    int l = inp[0].getInt(), r = inp[1].getInt();
    return roundIntoRange(l - r);
}

TimeSemantics::TimeSemantics(): Semantics({TINT, TINT}, TINT, "*") {}
Data TimeSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    if (inp[0].isError()) return inp[0];
    if (inp[1].isError()) return inp[1];
    int l = inp[0].getInt(), r = inp[1].getInt();
    return roundIntoRange(1ll * l * r);
}


MinSemantics::MinSemantics(): Semantics({TINT, TINT}, TINT, "min") {}
Data MinSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt(), r = inp[1].getInt();
    return new IntValue(std::min(l, r));
}

MaxSemantics::MaxSemantics(): Semantics({TINT, TINT}, TINT, "max") {}
Data MaxSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt(), r = inp[1].getInt();
    return new IntValue(std::max(l, r));
}

SquareSemantics::SquareSemantics(): Semantics({TINT}, TINT, "square") {}
Data SquareSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    if (inp[0].isError()) return inp[0];
    int l = inp[0].getInt();
    return new IntValue(l * l);
}

LqSemantics::LqSemantics(): Semantics({TINT, TINT}, TBOOL, "<") {}
Data LqSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt(), r = inp[1].getInt();
    return new BoolValue(l < r);
}

GqSemantics::GqSemantics(): Semantics({TINT, TINT}, TBOOL, ">") {}
Data GqSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt(), r = inp[1].getInt();
    return new BoolValue(l > r);
}

EvenSemantics::EvenSemantics(): Semantics({TINT}, TBOOL, "even") {}
Data EvenSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt();
    return new BoolValue(l % 2 == 0);
}

OddSemantics::OddSemantics(): Semantics({TINT}, TBOOL, "odd") {}
Data OddSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int l = inp[0].getInt();
    return Data(new BoolValue(l % 2 == 1));
}

LengthSemantics::LengthSemantics(): Semantics({TLIST}, TINT, "len") {}
Data LengthSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    std::vector<int> list = inp[0].getList();
    return new IntValue(list.size());
}

ApplyTwoSemantics::ApplyTwoSemantics(): Semantics({TSEMANTICS, TINT, TINT}, TINT, "apply2") {}
Data ApplyTwoSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    auto* semantics = dynamic_cast<SemanticsValue*>(inp[0].value);
    auto* f = semantics->semantics;
    auto result = f->run({std::move(inp[1]), std::move(inp[2])}, global_info);
    semantics->releaseSemantics();
    return std::move(result);
}

LtZeroSemantics::LtZeroSemantics(): Semantics({TINT}, TBOOL, "lt_zero") {}
Data LtZeroSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int x = inp[0].getInt();
    return new BoolValue(x < 0);
}

GtZeroSemantics::GtZeroSemantics(): Semantics({TINT}, TBOOL, "gt_zero") {}
Data GtZeroSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int x = inp[0].getInt();
    return new BoolValue(x > 0 );
}

LeqSemantics::LeqSemantics(): Semantics({TINT, TINT}, TBOOL, "<=") {}
Data LeqSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    int x = inp[0].getInt(), y = inp[1].getInt();
    return new BoolValue(x <= y);
}

IteIntSemantics::IteIntSemantics(): Semantics({TBOOL, TINT, TINT}, TINT, "ite") {}
Data IteIntSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    if (inp[0].getBool()) return std::move(inp[1]); else return std::move(inp[2]);
}

EqSemantics::EqSemantics(): Semantics({TINT, TINT}, TBOOL, "=") {}
Data EqSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return inp[0].getInt() == inp[1].getInt();
}

NeqSemantics::NeqSemantics(): Semantics({TINT, TINT}, TBOOL, "!=") {}
Data NeqSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return inp[0].getInt() != inp[1].getInt();
}

NotSemantics::NotSemantics(): Semantics({TBOOL}, TBOOL, "not") {}
Data NotSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return new BoolValue(!inp[0].getBool());
}

AndSemantics::AndSemantics(): Semantics({TBOOL, TBOOL}, TBOOL, "and") {}
Data AndSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return new BoolValue(inp[0].getBool() & inp[1].getBool());
}

OrSemantics::OrSemantics(): Semantics({TBOOL, TBOOL}, TBOOL, "or") {}
Data OrSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return new BoolValue(inp[0].getBool() | inp[1].getBool());
}

NegSemantics::NegSemantics(): Semantics({TINT}, TINT, "neg") {}
Data NegSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return -inp[0].getInt();
}

SingleListSemantics::SingleListSemantics(): Semantics({TINT}, TLIST, "[]") {}
Data SingleListSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return new ListValue({inp[0].getInt()});
}

ConsSemantics::ConsSemantics(): Semantics({TINT, TLIST}, TLIST, "cons") {}
Data ConsSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    LIST result = {inp[0].getInt()};
    for (auto& w: inp[1].moveList()) {
        result.push_back(w);
    }
    return new ListValue(std::move(result));
}

IntPassSemantics::IntPassSemantics(): Semantics({TINT}, TINT, "int") {}
Data IntPassSemantics::run(DataList&& inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    return inp[0];
}

AppendSemantics::AppendSemantics(): Semantics({TLIST, TINT}, TLIST, "append") {}
Data AppendSemantics::run(DataList &&inp, GlobalInfo* global_info) {
#ifdef DEBUG
    check(inp);
#endif
    LIST x = inp[0].moveList(); int y = inp[1].getInt();
    x.push_back(y);
    return new ListValue(std::move(x));
}

TailSemantics::TailSemantics(): Semantics({TLIST}, TLIST, "tail") {}
Data TailSemantics::run(DataList &&inp, GlobalInfo* global_info) {
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