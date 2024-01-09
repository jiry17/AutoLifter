//
// Created by pro on 2020/11/23.
//

#include "basic/config.h"

const std::string config::KSourcePath = "SOURCEPATH/";
const std::string config::KGrammarFilePath = config::KSourcePath + "resource/deepcoder_grammar.json";
const int config::KDefaultValue = 1000000000;
const int config::KIntMin = -5;
const int config::KIntMax = 5;
const int config::KVerifyIntMin = -10;
const int config::KVerifyIntMax = 10;
bool config::is_lifting_only = false;
int config::KMaxBranchNum = 6;
int config::KMaxTermNum = 5;
int config::KTermIntMax = 2;
int config::KRandomC = 5;
int config::KExampleIntMax = 100000000;
int config::KExtraTurnNum = 300;
std::vector<int> global::int_consts;
bool config::KIsVerbose = true;
int config::KVerifySize = 6;
int config::KVerifyOperatorLength = 3;