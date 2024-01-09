//
// Created by pro on 2024/1/3.
//

#include "exp/paradigm.h"
#include "basic/config.h"
#include <fstream>
#include "glog/logging.h"

std::string util::buildStructureDef(int n) {
    std::string s = "int";
    for (int i = 0; i < n; ++i) {
        if (i) s += ","; else s += " ";
        s += "x" + std::to_string(i);
    }
    return s;
}

std::string util::replaceToken(const std::string &original, const std::string &token, const std::string &s) {
    std::string result(original);
    while (1) {
        auto pos = result.find(token);
        if (pos == std::string::npos) break;
        result.replace(pos, token.length(), s);
    }
    return result;
}

namespace {
    std::vector<std::string> _readLines(const std::string& path) {
        std::string line; std::vector<std::string> result;
        std::fstream inf; inf.open(path.c_str(), std::ios::in);
        while (std::getline(inf, line)) {
            result.push_back(line);
        }
        return result;
    }
}

std::string util::readFile(const std::string &path) {
    std::string res; auto lines = _readLines(path);
    for (auto& line: lines) {
        if (!res.empty()) res += "\n";
        res += line;
    }
    return res;
}

namespace {
    const int KInitExampleNum = 100;

    Program* _synthesizeTrivialProgram(const std::vector<PointExample*>& example_list) {
        if (example_list.empty()) return new Program({}, new ConstSemantics(0));
        bool is_constant = true;
        for (auto* example: example_list) {
            if (example->second != example_list[0]->second) {is_constant = false; break;}
        }
        if (is_constant) return new Program({}, new ConstSemantics(example_list[0]->second));

        for (int i = 0; i < example_list[0]->first.size(); ++i) {
            bool is_id = true;
            for (auto* example: example_list) {
                if (example->first[i] != example->second) {is_id = false; break;}
            }
            if (is_id) return new Program({}, new ParamSemantics(i, example_list[0]->first[i].getType()));
        }
        return nullptr;
    }
}

Program *util::synthesisInitProgram(Program *lifting, int size, Task *task) {
    std::vector<PointExample*> example_list;
    auto example_config = task->example_space->c;
    std::uniform_int_distribution<int> dis(example_config.int_min, example_config.int_max);
    std::random_device d;
    for (int i = 0; i < KInitExampleNum; ++i) {
        std::vector<int> random_list(size);
        for (int j = 0; j < size; ++j) random_list[j] = dis(d);
        Data list_inp(new ListValue(random_list));
        auto oup = lifting->run({list_inp});
        DataList inp(size);
        for (int j = 0; j < size; ++j) inp[j] = Data(new IntValue(random_list[j]));
        example_list.push_back(new PointExample(inp, oup));
    }

    auto* trivial_program = _synthesizeTrivialProgram(example_list);
    if (trivial_program) return trivial_program;

    auto* solver = new PolyGen(task);
    auto final_program = solver->synthesis(example_list);
    delete solver;
    return final_program;
}

namespace {
    std::string _extractName(const std::string& s) {
        std::string res;
        for (auto& c: s) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) res += c;
        }
        return res;
    }
}

std::unordered_map<std::string, std::string> util::readBenchmarkFile(const std::string &path) {
    auto lines = _readLines(path);
    std::string pre_name, pre_content;
    std::unordered_map<std::string, std::string> result;
    for (auto& line: lines) {
        if (line.substr(0, 2) == "//") {
            if (!pre_content.empty()) result[pre_name] = pre_content;
            pre_name = _extractName(line);
            pre_content = "";
        } else {
            if (!pre_content.empty()) pre_content += "\n";
            pre_content += line;
        }
    }
    if (!pre_content.empty()) result[pre_name] = pre_content;
    return result;
}

namespace {
    int _getVerifyIntMin(int k) {
        if (k == config::KIntMin) return config::KVerifyIntMin;
        return k;
    }

    int _getVerifyIntMax(int k) {
        if (k == config::KIntMax) return config::KVerifyIntMax;
        return k;
    }
}

std::string util::replaceDefaultTokens(const std::string &original, Task *task, int aux_num) {
    auto result = original;
    result = replaceToken(result, "ResultDefineString", buildStructureDef(aux_num));
    result = replaceToken(result, "KArrayLength", std::to_string(config::KVerifySize));
    result = replaceToken(result, "KIntMin", std::to_string(_getVerifyIntMin(task->example_space->c.int_min)));
    result = replaceToken(result, "KIntMax", std::to_string(_getVerifyIntMax(task->example_space->c.int_max)));
    result = replaceToken(result, "KINF", std::to_string(config::KDefaultValue));
    result = replaceToken(result, "KOperationLength", std::to_string(config::KVerifyOperatorLength));
    return result;
}

namespace {

    std::string _wrap(const std::string &s) {
        return "(" + s + ")";
    }

    std::string _unaryProgram2CString(Program *program, const std::vector<std::string> &param_names) {
        static const std::unordered_set<std::string> op_list = {"not", "is_even", "int"};
        auto op_name = program->semantics->name;
        if (op_list.find(op_name) == op_list.end()) {
            LOG(FATAL) << "Unknown operator " << op_name;
        }
        if (op_name == "not") {
            return "!(" + util::program2CString(program->sub_list[0], param_names) + ")";
        } else if (op_name == "is_even") {
            auto sub_string = util::program2CString(program->sub_list[0], param_names);
            return "(" + sub_string + ") & 1";
        } else if (op_name == "int") {
            return util::program2CString(program->sub_list[0], param_names);
        }
        assert(0);
    }

    std::string _binaryProgram2CString(Program *program, const std::vector<std::string> &param_names) {
        auto l_name = _wrap(util::program2CString(program->sub_list[0], param_names));
        auto r_name = _wrap(util::program2CString(program->sub_list[1], param_names));
        static std::unordered_set<std::string> KUnchangedOp = {
                "+", "-", "*", "<=", "<", ">=", ">", "==", "/", "!="
        };
        static std::unordered_map<std::string, std::string> KRenamedOp = {
                {"and", "&&"},
                {"or",  "||"},
                {"div", "/"},
                {"=",   "=="}
        };
        auto op_name = program->semantics->name;
        if (KUnchangedOp.find(op_name) != KUnchangedOp.end()) {
            return l_name + op_name + r_name;
        } else if (KRenamedOp.find(op_name) != KRenamedOp.end()) {
            return l_name + KRenamedOp.find(op_name)->second + r_name;
        }
        LOG(FATAL) << "Unknown operator " << op_name;
    }

    std::string _trinaryProgram2CString(Program *program, const std::vector<std::string> &param_names) {
        auto op_name = program->semantics->name;
        if (op_name != "ite") {
            LOG(FATAL) << "Unknown trinary operator " << op_name;
        }
        auto c = _wrap(util::program2CString(program->sub_list[0], param_names));
        auto t = _wrap(util::program2CString(program->sub_list[1], param_names));
        auto f = _wrap(util::program2CString(program->sub_list[2], param_names));
        return c + "?" + t + ":" + f;
    }
}


std::string util::program2CString(Program *program, const std::vector<std::string> &param_names) {
    auto* ps = dynamic_cast<ParamSemantics*>(program->semantics);
    if (ps) {
        return param_names[ps->getId()];
    }
    auto* cs = dynamic_cast<ConstSemantics*>(program->semantics);
    if (cs) {
        assert(cs->value.getType() == Type::TINT);
        return std::to_string(cs->value.getInt());
    }
    assert(1 <= program->sub_list.size() && program->sub_list.size() <= 3);
    if (program->sub_list.size() == 1) {
        return _unaryProgram2CString(program, param_names);
    } else if (program->sub_list.size() == 2) {
        return _binaryProgram2CString(program, param_names);
    } else if (program->sub_list.size() == 3) {
        return _trinaryProgram2CString(program, param_names);
    }
    assert(0);
}

std::string
util::synthesisResult2CString(const MergeInfo &info, const std::function<std::string(const std::string &)> &name_map) {
    std::vector<std::string> param_list;
    for (auto& param_name: info.param_name) {
        param_list.push_back(name_map(param_name));
    }
    return util::program2CString(info.merged_program, param_list);
}

std::string util::mergeComponent(const std::vector<std::string>& component_list) {
    std::string result = "(result){";
    for (int i = 0; i < component_list.size(); ++i) {
        if (i) result += ",";
        result += component_list[i];
    }
    return result + "}";
}

namespace {
    std::string _getHashName(const std::string& program) {
        int hash = 0;
        const int key = 10007;
        for (auto c: program) hash = hash * key + int(c);
        return std::to_string(hash);
    }
}

void util::verifyViaCBMC(const std::string &program,  int unwind_num, bool is_unwind_check) {
    auto prefix = _getHashName(program);
    auto verify_inp = "/tmp/" + prefix + ".cpp";
    auto verify_oup = "/tmp/" + prefix + ".out";

    auto* f = fopen(verify_inp.c_str(), "w");
    fprintf(f, "%s", program.c_str());
    fclose(f);

    std::string command = "cbmc " + verify_inp;
    if (unwind_num != -1) command += " --unwind " + std::to_string(unwind_num);
    if (is_unwind_check) command += " --unwinding-assertions";
    command += " --verbosity 5";
    command += " > " + verify_oup;
    LOG(INFO) << "Try verify file " << verify_inp;
    LOG(INFO) << command << std::endl;
    std::system(command.c_str());

    auto verify_res = readFile(verify_oup);

    if (verify_res.find("SUCCESS") == std::string::npos || verify_res.find("FAILURE") != std::string::npos) {
        LOG(FATAL) << "Verify Failed " << verify_res;
    }
    std::system(("rm " + verify_inp).c_str());
    std::system(("rm " + verify_oup).c_str());
}
