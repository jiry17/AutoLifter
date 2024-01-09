#include <cassert>
int nondet_int();
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a < b ? b : a;
}
const int n = KArrayLength;
int w[n + 10], rpos[n + 10];
struct result {
    ResultDefineString;
}info[n + 10];

ReferenceProgram

result mergeRes(int a, const result& lres, const result& rres, int lsize, int rsize) {
    if (lsize) {
        if (rsize) {
            FullMergeString
            return res_info;
        } else {
            RightMergeString
            return res_info;
        }
    } else if (rsize) {
        LeftMergeString
        return res_info;
    } else {
        return OneCaseString;
    }
}

int lsp() {
    int num = 0;
    for (int i = 1; i <= n; ++i) {
        result rres = ZeroCaseString;
        while (num > 0 && w[rpos[num]] OperatorString w[i]) {
            result lres = info[num]; int a = w[rpos[num]];
            rres = mergeRes(a, lres, rres, rpos[num] - rpos[num - 1] - 1, i - rpos[num] - 1);
            --num;
        }
        num++; rpos[num] = i; info[num] = rres;
    }
    result rres = ZeroCaseString;
    for (int i = num; i; --i) {
        result lres = info[i]; int a = w[rpos[i]];
        rres = mergeRes(a, lres, rres, rpos[i] - rpos[i - 1] - 1, n - rpos[i]);
    }
    return rres.x0;
}

int main() {
    for (int i = 1; i <= n; ++i) {
        w[i] = nondet_int();
        __CPROVER_assume(KIntMin <= w[i] && w[i] <= KIntMax);
    }
    int ref = oracle();
    int myres = lsp();
    assert(ref == myres);
}