#include <iostream>

#include "complete_solver.h"
#include "benchmark.h"
#include "pure_observational_equivalent.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "enumerate_based_solver.h"
#include "brute_force_complete_solver.h"
#include "hfta_complete_solver.h"
#include "recorder.h"

DEFINE_string(type, "", "The type of the task (list/tree/operator)");
DEFINE_string(name, "", "The name of the benchmark");
DEFINE_string(oup, "", "The path of the output file");
DEFINE_string(log, "", "The path of the log");
DEFINE_string(solver, "", "Lifting solver (std/bf)");

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    std::string domain, name, operator_name, log_file, oup_file;

    if (FLAGS_type.empty()) {
        domain = "listr";
        name = "mss";
        FLAGS_solver = "std";
    } else {
        domain = FLAGS_type;
        name = FLAGS_name;
        log_file = FLAGS_log;
        oup_file = FLAGS_oup;
    }

    if (domain == "listr") {
        benchmark::KIsSinglePass = true;
    }
    if (!log_file.empty()) {
        google::SetLogDestination(google::GLOG_INFO, log_file.c_str());
    } else {
        FLAGS_logtostderr = true;
    }

    if (!oup_file.empty()) {
        freopen(oup_file.c_str(), "w", stdout);
        config::oup_file = oup_file;
    }

    std::vector<Task *> task_list = benchmark::getBenchmark(domain, name);
    recorder::recorder.setName(name);

    LiftingSolver *s = nullptr;
    if (FLAGS_solver == "mbf") {
        s = new BfLiftingSolver(task_list);
    } else if (FLAGS_solver == "relish") {
        s = new HFTACompleteSolver(task_list);
    } else {
        SolverBuilder *builder = nullptr;
        if (FLAGS_solver == "std") {
            builder = new EnumerateBasedBuilder();
        } else if (FLAGS_solver == "poe") {
            builder = new ObservationalEquivalenceSolverBuilder();
        } else
            assert(0);

        s = new CompleteSolver(task_list, builder, new ExternalSolverBuilder());
    }
    LOG(INFO) << "start";
    s->synthesis();
    LOG(INFO) << "success";
    std::cout << "Success" << std::endl;
    s->printSummary();
    return 0;
}
