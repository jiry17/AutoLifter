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
    int res = 0, len = 0; result info = ZeroCaseString;
    for (int i = 1; i <= n; ++i) {
        int a = w[i];
        MergeCaseString
        if (!info.x0) {
            info = OneCaseString;
            if (info.x0) {
                len  = 1;
            } else {
                len = 0; info = ZeroCaseString;
            }
        } else {
            len += 1;
        }
        res = max(res, len);
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