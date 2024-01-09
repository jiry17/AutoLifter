//
// Created by jiruy on 2020/11/21.
//

#ifndef CPP_SEMANTICS_FACTORY_H
#define CPP_SEMANTICS_FACTORY_H

#include "semantics.h"
#include "config.h"

#define DefineSemantics(name) \
class name ## Semantics: public Semantics { \
public: \
    name ## Semantics();      \
    virtual Data run(DataList&& inp, GlobalInfo* global_info); \
}

DefineSemantics(Link);
DefineSemantics(Head);
DefineSemantics(Last);
DefineSemantics(Take);
DefineSemantics(Drop);
DefineSemantics(Access);
DefineSemantics(Minimum);
DefineSemantics(Maximum);
DefineSemantics(Reverse);
DefineSemantics(Sort);
DefineSemantics(Sum);
DefineSemantics(Map);
DefineSemantics(Filter);
DefineSemantics(Count);
DefineSemantics(Zipwith);
DefineSemantics(Scanl);
DefineSemantics(Scanr);
DefineSemantics(Plus);
DefineSemantics(Minus);
DefineSemantics(Time);
DefineSemantics(Min);
DefineSemantics(Max);
DefineSemantics(Square);
DefineSemantics(Lq);
DefineSemantics(Gq);
DefineSemantics(Even);
DefineSemantics(Odd);
DefineSemantics(Length);
DefineSemantics(ApplyTwo);
DefineSemantics(LtZero);
DefineSemantics(GtZero);
DefineSemantics(Leq);
DefineSemantics(IteInt);
DefineSemantics(Eq);
DefineSemantics(Neq);
DefineSemantics(Not);
DefineSemantics(And);
DefineSemantics(Or);
DefineSemantics(Neg);
DefineSemantics(SingleList);
DefineSemantics(Cons);
DefineSemantics(IntPass);
DefineSemantics(Append);
DefineSemantics(Tail);

namespace semantics {
    Semantics* getSemanticsFromName(std::string name);
}

#endif //CPP_SEMANTICS_FACTORY_H
