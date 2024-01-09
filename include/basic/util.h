//
// Created by pro on 2020/12/7.
//

#ifndef CPP_UTIL_H
#define CPP_UTIL_H

#include "data.h"
#include "program.h"

namespace util {
    std::string dataList2String(const DataList& data);
    std::string getDefaultSymbolForType(Type type);
    Program* buildParamProgram(int id, Type type);
}


#endif //CPP_UTIL_H
