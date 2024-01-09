//
// Created by pro on 2021/5/5.
//

#ifndef CPP_POLYGEN_H
#define CPP_POLYGEN_H

#include "polygen/term_solver.h"
#include "autolifter/external_comb_solver.h"
#include "polygen/unifier.h"

class PolyGen {
    TermSolver* term_solver;
    Unifier* unifier;
public:
    PolyGen(Task* _task): term_solver(new TermSolver(_task)), unifier(new Unifier(_task)) {
    }
    void clearCache() {
        term_solver->clearCache();
    }
    Program* synthesis(const std::vector<PointExample*>& example_list);
    Program* phasedSynthesis(const std::vector<PointExample*>& example_list);
};


#endif //CPP_POLYGEN_H
