#ifndef L2S_SPECIFICATION_H
#define L2S_SPECIFICATION_H

#include "semantics.h"
#include "program.h"

#include <map>
#include <iostream>

class Specification;
class Rule;

// A non-terminal symbol in a DSL.
class NonTerminal {
public:
    std::string name;
    Type type;
    std::vector<Rule*> rule_list;
    int id;
    NonTerminal(std::string _name, Type _type): name(_name), type(_type), id(0) {}
};

// A grammar rule in a DSL.
class Rule {
    void check() {
        assert(semantics->inp_type_list.size() >= param_list.size());
        for (int i = 0; i < param_list.size(); ++i) {
            assert(semantics->inp_type_list[i] == param_list[i]->type);
        }
    }
public:
    Semantics* semantics;
    std::vector<NonTerminal*> param_list;
    Rule(Semantics* _semantics, const std::vector<NonTerminal*> _param_list): semantics(_semantics), param_list(_param_list) {
#ifdef DEBUG
        check();
#endif
    }
};

class Grammar {
    NonTerminal* searchByName(std::string name);
public:
    NonTerminal* start_symbol;
    std::vector<NonTerminal*> symbol_list;
    Grammar(NonTerminal* _start_symbol, const std::vector<NonTerminal*>& _symbol_list): start_symbol(_start_symbol), symbol_list(_symbol_list) {}
    void addParam(const std::vector<Type>& type_list);
    void indexSymbols();
    void print() const;
};

namespace grammar{
    Grammar* getDeepCoderDSL(bool is_single = true);
}




#endif //L2S_SPECIFICATION_H
