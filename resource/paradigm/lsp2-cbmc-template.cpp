#include <cassert>
int nondet_int();
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a < b ? b : a;
}
const int n = KArrayLength;
int w [n + 10];
struct result {
    ResultDefineString;
};

ReferenceProgram

int lsp() {
    int res = 0, l = 1;
    result info = ZeroCaseString;
    for (int r = 1; r <= n; ++r) {
        int a = w[r];
        PushCaseString
        while (l <= r && !info.x0) {
            a = w[l];
            PopCaseString
            l += 1;
        }
        res = max(res, r - l + 1);
    }
    return res;
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