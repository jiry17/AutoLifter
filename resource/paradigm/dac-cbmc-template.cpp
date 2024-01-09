#include <cassert>
int nondet_int();
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a > b ? a : b;
}
const int n = KArrayLength;
int w[n + 10];

struct result {
    ResultDefineString;
};

result dac(int l, int r) {
    if (l == r) return InitCaseString;
    int mid = (l + r) / 2;
    result lres = dac(l, mid), rres = dac(mid + 1, r);
    return MergeCaseString;
}

ReferenceProgram

int main() {
    for (int i = 1; i <= n; ++i) {
        w[i] = nondet_int();
        __CPROVER_assume(KIntMin <= w[i] && w[i] <= KIntMax);
    }
    assert(oracle() == dac(1, n).x0);
}