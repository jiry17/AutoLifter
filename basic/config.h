//
// Created by pro on 2020/11/23.
//

#ifndef CPP_CONFIG_H
#define CPP_CONFIG_H

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <gurobi_c++.h>

namespace config {
    extern const std::string KSourcePath;
    extern const std::string KGrammarFilePath;
    extern const int KDefaultValue;
    extern const int KIntMin;
    extern const int KIntMax;
    extern bool is_lifting_only;
    extern int KMaxBranchNum;
    extern int KExampleIntMax;
    extern int KMaxTermNum;
    extern int KTermIntMax;
    extern int KRandomC;
    extern bool KIsRelishNoArith;
    extern int KExtraTurnNum;
    extern std::string oup_file;
}

namespace global {
    extern std::vector<int> int_consts;
}


#endif //CPP_CONFIG_H
