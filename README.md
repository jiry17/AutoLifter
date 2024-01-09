# AutoLifter

Artifact for TOPLAS24: "[Decomposition-Based Synthesis for Applying D&C-Like Algorithmic Paradigms](https://jiry17.github.io/paper/TOPLAS24.pdf)".

### Install

#### Build from source (Test on Ubuntu 16.04/20.04)

1. Install dependencies. gcc $\geq$ 9, CMake $\geq$ 3.13, and python3 are required to build the project. The other dependencies can be installed as the following.

   ```bash
   apt-get install libjsoncpp-dev libgoogle-glog-dev cbmc
   pip3 install tqdm argparse
   ```

2. Clone *AutoLifter* from this repository.

   ```bash
   git clone https://github.com/jiry17/AutoLifter.git
   ```

3. Build the whole project.

   ```bash
   cd AutoLifter; ./install
   ```

4. *AutoLifter* uses [*PolyGen*](https://github.com/jiry17/PolyGen) as a client synthesizer for synthesizing integer arithmetic expressions. The implementation of *PolyGen* we use takes *guropi* as the underlying ILP solver, so a *gurobi* license is required to run the full functionality of *AutoLifter*. You can get an academic license via the following steps.

   1. Register or log in at the [webtide](https://www.gurobi.com/) of gurobi.
   2. Visit the [Free Academic License page](https://www.gurobi.com/downloads/end-user-license-agreement-academic/).
   3. Click ***I Accept These Conditions***.
   4. Get a command like `grbgetkey x...x` at the bottom of the webpage.
   5. Replace `grbgetkey` with `resource/gurobi912/linux64/bin/grbgetkey` and execute this command under the root directory of *AutoLifter*.
   6. Test whether the license works normally by executing `thirdparty/gurobi912/linux64/bin/gurobi.sh` under the root directory of the project.

#### Run tests

1. Test whether the project is successfully built.

   ```bash
   cd run; ./run -p dac -t sum --solver Relish
   ```

   The last few lines are expected to be `Success` followed by some synthesis results and some statistics of the synthesis, as shown below, where `??` should be some float numbers representing the time cost of components in *AutoLifter*.

   ```
   Success
   x0: (sum Param0)
   Liftings:
   Combinator for dac
     x0: l0+r0
   |               |       cbmc |  synthesis |   #lifting |
   |            sum|         ?? |         ?? |          0 |
   Total time cost: ??
   ```

   This result demonstrates a divide-and-conquer algorithm for calculating the sum of an integer list. 

   1. Line `x0: (sum Param0)` shows the target function, i.e., the sum of a list.
   2. Paragraph `Liftings:` lists those auxiliary values found by *AutoLifter*. In this example, no auxiliary value is needed so this paragraph is empty.
   3. Paragraph `Combinator for dac` shows how to calculate the target function and the auxiliary values via divide-and-conquer. Here, `x0: l0 + r0` shows that the sum of the whole list is equal to the sum of the left half plus the sum of the right half.
   4. In the table, entry `cbmc` and `synthesis` show the time costs of verification and synthesis, respectively, and entry `#lifting` shows the number of synthesized auxiliary values.
   5. At last, Line `Total time cost:` shows the total execution time of *AutoLifter*.

2. Test whether `gurobi` is correctly configured.

   ```
   cd run; ./run -p dac -t mps --solver AutoLifter
   ```

   The last few lines of the output are expected to be as follows. There will be three new entries in the last table: `external`, `lifting`, and `#turn`. They represent the time costs of synthesizing the combinator and the auxiliary values, and the number of used CEGIS iterations, respectively. 

   ```
   Success
   x0: (mps Param0)
   Liftings:
     x1: (sum Param0)
   Combinator for dac
     x0: ite (l0<=l1+r0) (l1+r0) (l0)
     x1: l1+r1
   |               |       cbmc |   external |    lifting |   #lifting |      #turn |
   |            mps|         ?? |         ?? |         ?? |          1 |          6 |
   Total time cost: ??
   ```

### Run AutoLifter

#### Run a Task in Our Dataset

We provide a script `run/run` to run *AutoLifter* on a single task in our dataset.

```
run [-h] --problem {dac,single-pass,longest-segment,segment-tree} 
		 --task TASK [--output OUTPUT] [--runnable RUNNABLE] 
         [-solver {AutoLifter,OE,Relish,ESolver}]
```

This script takes the following parameters.

|    Flag    |                  Effect                  |   Default    |
| :--------: | :--------------------------------------: | :----------: |
| `problem`  |  The problem type related to the task.   |      NA      |
|   `task`   | The name of the task. All tasks in our dataset are pre-defined, and their types and names are summarized in `run/benchmark.json` |      NA      |
|  `output`  | The path to print a summary of the synthesis procedure. |    stdout    |
| `runnable` | The path to print the runnable program (for the given algorithmic task) corresponding to the synthesis result. |     None     |
|  `solver`  |       The name of the synthesizer.       | *AutoLifter* |

#### Run a New Task

To run *AutoLifter* on a new task in a known type, you need to register this task in the corresponding file in directory `exp/dataset`, for example, all divide-and-conquer tasks in our dataset have already been registered in `exp/dataset/divide-and-conquer.cpp`. Script `run/run` can invoke *AutoLifter* on all registered tasks.

To run *AutoLifter* on a new algorithmic paradigm, you need to declare the reduction from applying this paradigm to lifting problems. Specifically,  you need to instantiate the abstract class `CBMCParadigm` (defined in file `include/exp/paradigm.h`) by implementing the following two methods.

1. `std::vector<TaskType*> getTaskTypes() const` returns the reduced lifting problem.
2. `std::string buildRunnableProgram(LiftingSolver* solver) const`  converts the synthesis result of the lifting problem (stored in `solver`) to a full runnable program (e.g., a c++ program).
3. `void verifyViaCBMC(LiftingSolver* solver, const std::string& source_path) const` verifies the synthesis result in `solver` against a reference implementation in `source_path` via CBMC.

The current implementation of *AutoLifter* supports 6 different paradigms. Their implementations can be found in `exp/paradigms`.

### Run Experiments

We provide a script `run/run_exp` to draw the tables in our paper and reproduce the evaluation results.

```
run_exp [-h] [--exp {1,2,3,4}] [--restart] [--time-limit TIME_LIMIT] 
		     [--memory-limit MEMORY_LIMIT]
```

This script takes the following parameters.

|      Flag      |                  Effect                  | Default  |
| :------------: | :--------------------------------------: | :------: |
|     `exp`      | The index of the experiment to be reproduced. |   All    |
|   `restart`    | To clear the cached results. `run_exp` will cache all results under directory `run/result_cache`, where the original results we used in our paper are stored initially. Enabling this flag will clear all cache files. | disabled |
|  `time-limit`  | The time limit of each execution (second). |   300    |
| `memory-limit` | The memory limit of each execution (GB). |    8     |

**Note:** `run_exp` caches the results of all finished executions. if `run_exp` is interrupted exceptionally, re-running `run_exp` without flag `--restart` will skip all finished executions. 

### Outline of the Source Code

The source code of this project is organized as below. 

|  Directory   |               Description                |
| :----------: | :--------------------------------------: |
|  `include`   | The`.h` files of this project. They are organized in the same way as the `.cpp` files. |
|   `basic`    | Those basic data structures required to describe a synthesis task, such as values (`values.cpp`), semantics (`semantics.cpp`), and programs (`program.cpp`). |
| `autolifter` | The implementation of *AutoLifter*, where `lifting_problem.cpp` declares the lifting problems, and `complete_solver.cpp` and `observational_covering.cpp` implement the decomposition system and observational covering, respectively. |
|  `polygen`   | The external synthesizer *PolyGen* for synthesizing combinators. |
|    `exp`     | Programs used in our evaluation, where files in  `dataset` and `paradigms` declare the tasks and paradigms in our dataset. |
|  `baseline`  | The baseline solvers considered in our evaluation, where `esolver`, `relish`, and `oe` implement *ESolver*, *Relish*, and *AutoLifter$_{\text{OE}}$*, respectively. |
|  `resource`  | Auxiliary files used in our implementation, where `deepcoder_grammar.json` declares the *DeepCoder*'s grammar, `dataset` includes the reference programs of all tasks in our dataset, and `paradigm` includes the c++ template of paradigms considered in our evaluation. |
