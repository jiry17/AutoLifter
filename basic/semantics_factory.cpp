//
// Created by jiruy on 2020/11/21.
//

#include "semantics_factory.h"
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
            {"filter_index", new FilterIndexSemantics()}, {"not", new NotSemantics()},
            {"and", new AndSemantics()}, {"or", new OrSemantics()},
            {"*", new TimeSemantics()}, {"pow", new PowSemantics()},
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