tasks_requiring_extra_operators = {
    "dad": ["atoi", "count1(0*)2", "max_sum_between_ones", "lis", "largest_peak", "longest1(0*)2",
            "longest_odd(10)*"],
    "listr": ["lis", "largest_peak", "max_sum_between_ones", "longest_odd(10)*", "mmm", "mas"],
    "seg": ["page21"],
    "ds": []
}

import os
import json

def get_result(name, is_extra = False):
    path = os.path.join("result_cache", name + ".json")
    with open(path, "r") as inp:
        result = json.load(inp)
    res = {}
    fail = []
    sname = name
    for dname, d in result.items():
        for name, r in d.items():
            full_name = dname + "@" + name
            if r["status"] == "success":
                if name in tasks_requiring_extra_operators[dname] and not is_extra:
                    fail.append(full_name)
                    continue
                res[full_name] = (r["time"]["total"], r["lift_num"] - 1)
            else:
                fail.append(full_name)
    return res, fail
    
def compare_with_dpa(dpa_name):
    success1, fail1 = get_result("AutoLifter", False)
    res1 = {}
    for name in success1:
        if "listr" in name:
            res1[name] = success1[name]

    with open("result_cache/" + dpa_name + ".json", "r") as inp:
        res2 = json.load(inp)

    for name in res2:
        assert "listr@" + name in success1 or "listr@" + name in fail1

    solved1, solved2, common, sum1, sum2 = len(res1), 0, 0, 0, 0
    for name, res in res2.items():
        if res["status"]:
            solved2 += 1
            full_name = "listr@" + name
            if full_name in res1:
                common += 1
                sum1 += res1[full_name][0]
                sum2 += res["time"]

    print("compare std with dpa jointly solved:", common)
    print("std #solved", solved1, "time:", sum1 / common)
    print("dpa #solved", solved2, "time:", sum2 / common)


def cmp(name_1, name_2, is_extra=False):
    res_1, _ = get_result(name_1, is_extra)
    res_2, _ = get_result(name_2, is_extra)

    for group in ["", "dad", "listr", "seg", "ds"]:
        jointly_solved = 0
        total_time = [0, 0]
        total_lifting = [0, 0]
        solve_1, solve_2 = 0, 0
        for name in res_1:
            if (group + "@") in name: solve_1 += 1
        for name in res_2:
            if (group + "@") in name: solve_2 += 1
        for name, r_1 in res_1.items():
            if name in res_2:
                if (group + "@") not in name: continue
                r_2 = res_2[name]
                jointly_solved += 1

                total_time[0] += r_1[0]
                total_time[1] += r_2[0]
                total_lifting[0] += r_1[1]
                total_lifting[1] += r_2[1]

        print("Group", group)
        print("compare", name_1, "with", name_2, "jointly solved:", jointly_solved)
        print(name_1, "#solved:", solve_1, "time:", total_time[0] / jointly_solved, "#lifting:", total_lifting[0] / jointly_solved)
        print(name_2, "#solved:", solve_2, "time:", total_time[1] / jointly_solved, "#lifting:", total_lifting[1] / jointly_solved)

def get_parsynt_result(name):
    path = os.path.join("result_cache", name + ".json")
    with open(path, "r") as inp:
        result = json.load(inp)
    res = {}
    for d in result.values():
        for name, r in d.items():
            if "status" in r and r["status"] == "fail": continue
            for k in r: assert k in ["lift_num", "given_num", "time", "status"]
            res["dad@" + name] = (r["time"], r["lift_num"], 0 if "given_num" not in r else r["given_num"])
    return res

def cmp_with_parsynt(name_1, name_2, is_extra=False):
    res_1 = get_parsynt_result(name_1)
    res_2, fail_2 = get_result(name_2, is_extra)
    res = res_2
    res_2 = {}
    for a, b in res.items():
        if "dad" in a: res_2[a] = b
    jointly_solved = 0
    lifting_num = [0, 0]
    extra_lifting = 0
    time_cost = [0, 0]
    for name, r_1 in res_1.items():
        assert name in res_2 or name in fail_2
        if name not in res_2: continue
        r_2 = res_2[name]

        jointly_solved += 1
        time_cost[0] += r_1[0]
        lifting_num[0] += r_1[1]
        extra_lifting += r_1[2]

        time_cost[1] += r_2[0]
        lifting_num[1] += r_2[1]


    print("compare", name_1, "with", name_2, "jointly solved:", jointly_solved)
    print(name_1, "#solved:", len(res_1), "time:", time_cost[0] / jointly_solved, "#lifting:", lifting_num[0] / jointly_solved, "#extra_lifting:", extra_lifting / jointly_solved, "ratio:", lifting_num[0] / (lifting_num[0] + extra_lifting))
    print(name_2, "#solved:", len(res_2), "time:", time_cost[1] / jointly_solved, "#lifting:", lifting_num[1] / jointly_solved)


if __name__ == "__main__":
    print("RQ1")
    cmp("AutoLifter", "AutoLifter")
    cmp("ESolver", "AutoLifter")
    cmp("Relish", "AutoLifter")
    
    print("\n\n\n")
    print("RQ2")
    cmp_with_parsynt("Parsynt17", "AutoLifter")
    cmp_with_parsynt("Parsynt21", "AutoLifter")

    print("\n\n\n")
    print("RQ3")
    compare_with_dpa("DPASynth")
    compare_with_dpa("DPASynth+")

    print("\n\n\n")
    print("RQ4")
    cmp("AutoLifterOE", "AutoLifter")
