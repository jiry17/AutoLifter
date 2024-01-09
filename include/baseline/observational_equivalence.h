//
// Created by pro on 2021/6/28.
//

#ifndef CPP_OBSERVATIONAL_EQUIVALENCE_H
#define CPP_OBSERVATIONAL_EQUIVALENCE_H

#include "autolifter/solver.h"

class ObservationalEquivalenceAuxSolver: public Solver {
    Program* synthesisFromExample(const std::vector<std::pair<Example*, Example*>>& example_list);
public:
    ObservationalEquivalenceAuxSolver(Task *_task): Solver(_task) {}
    virtual ~ObservationalEquivalenceAuxSolver() = default;
    virtual std::vector<Program*> synthesis();
};

class ObservationalEquivalenceSolverBuilder: public SolverBuilder {
public:
    virtual Solver * build(Task *task) {
        return new ObservationalEquivalenceAuxSolver(task);
    }
};


#endif //CPP_OBSERVATIONAL_EQUIVALENCE_H
