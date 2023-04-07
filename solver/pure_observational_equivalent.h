//
// Created by pro on 2021/6/28.
//

#ifndef CPP_PURE_OBSERVATIONAL_EQUIVALENT_H
#define CPP_PURE_OBSERVATIONAL_EQUIVALENT_H

#include "solver.h"

class ObservationalEquivalenceSolver: public Solver {
    Program* synthesisFromExample(const std::vector<std::pair<Example*, Example*>>& example_list);
public:
    ObservationalEquivalenceSolver(Task *_task): Solver(_task) {}
    virtual ~ObservationalEquivalenceSolver() = default;
    virtual std::vector<Program*> synthesis();
};

class ObservationalEquivalenceSolverBuilder: public SolverBuilder {
public:
    virtual Solver * build(Task *task) {
        return new ObservationalEquivalenceSolver(task);
    }
};


#endif //CPP_PURE_OBSERVATIONAL_EQUIVALENT_H
