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

ReferenceProgram

int main() {
    for (int i = 1; i <= n; ++i) {
        w[i] = nondet_int();
        __CPROVER_assume(KIntMin <= w[i] && w[i] <= KIntMax);
    }
    int a = w[1];
    result res = InitCaseString;
    for (int i = 2; i <= n; ++i) {
        a = w[i];
        MergeCaseString
    }
    int ref = oracle();
    int myres = res.x0;
    assert(ref == myres);
}