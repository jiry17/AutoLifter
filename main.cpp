#include <iostream>

#include "autolifter/complete_solver.h"
#include "exp/benchmark.h"
#include "baseline/observational_equivalence.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "basic/semantics_factory.h"
#include "autolifter/observational_covering.h"
#include "baseline/esolver.h"
#include "baseline/relish.h"
#include "basic/config.h"
#include "include/basic/recorder.h"

DEFINE_string(problem, "", "The name of the problem (dac/single-pass/longest-segment/segment-tree)");
DEFINE_string(name, "", "The name of the benchmark");
DEFINE_string(oup, "", "The path of the output file");
DEFINE_string(runnable, "", "The output path of the resulting runnable files");
DEFINE_string(solver, "", "The name of the solver (AutoLifter/ESolver/Relish/OE)");
DEFINE_bool(verbose, false, "Output complete log");

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_logtostderr = true;

    std::string problem, name, operator_name, log_file, oup_file;
    problem = FLAGS_problem;
    name = FLAGS_name;
    oup_file = FLAGS_oup;
    config::KIsVerbose = FLAGS_verbose;

    auto benchmark_info = benchmark::getBenchmark(problem, name);
    recorder::recorder.setName(name);

    LiftingSolver *s;
    if (FLAGS_solver == "ESolver") {
        s = new ESolver(benchmark_info.tasks);
    } else if (FLAGS_solver == "Relish") {
        s = new Relish(benchmark_info.tasks);
    } else {
        SolverBuilder *builder;
        if (FLAGS_solver == "AutoLifter") {
            builder = new EnumerateBasedBuilder();
        } else if (FLAGS_solver == "OE") {
            builder = new ObservationalEquivalenceSolverBuilder();
        } else
            assert(0);
        s = new CompleteSolver(benchmark_info.tasks, builder, new ExternalSolverBuilder());
    }
    s->synthesis();

    recorder::recorder.pushStage("cbmc");
    {
        auto* cbmc_paradigm = dynamic_cast<CBMCParadigm*>(benchmark_info.paradigm);
        cbmc_paradigm->verifyViaCBMC(s, benchmark_info.source_path);
    }
    recorder::recorder.popStage();

    if (!oup_file.empty()) {
        freopen(oup_file.c_str(), "w", stdout);
    }
    std::cout << "Success" << std::endl;
    s->printSummary();

    if (!FLAGS_runnable.empty()) {
        auto res = benchmark_info.paradigm->buildRunnableProgram(s);
        auto* f = fopen(FLAGS_runnable.c_str(), "w");
        fprintf(f, "%s", res.c_str());
        fclose(f);
    }
    return 0;
}
