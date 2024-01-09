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
    extern const int KVerifyIntMin;
    extern const int KVerifyIntMax;
    extern bool is_lifting_only;
    extern int KMaxBranchNum;
    extern int KExampleIntMax;
    extern int KMaxTermNum;
    extern int KTermIntMax;
    extern bool KIsVerbose;
    extern int KRandomC;
    extern int KExtraTurnNum;
    extern int KVerifySize;
    extern int KVerifyOperatorLength;
}

namespace global {
    extern std::vector<int> int_consts;
}


#endif //CPP_CONFIG_H
