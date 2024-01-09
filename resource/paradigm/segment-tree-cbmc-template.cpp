#include <cassert>
int nondet_int();
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a < b ? b : a;
}
const int n = 4, m = 2;
int w[n + 10], tag[(n << 2) + 10];
struct result {
    ResultDefineString;
}info[n + 10];
void apply(int pos, int a) {
    result res = info[pos];
    tag[pos] = UpdateTagString;
    UpdateString
    info[pos] = res_info;
}
void pushdown(int pos) {
    if (tag[pos] == DefaultTag) return;
    apply(pos * 2, tag[pos]);
    apply(pos * 2 + 1, tag[pos]);
    tag[pos] = DefaultTag;
}
void init(int pos, int l, int r) {
    tag[pos] = DefaultTag;
    if (l == r) {
        int a = w[l]; info[pos] = OneCaseString;
        return;
    }
    int mid = l + r >> 1;
    init(pos * 2, l, mid);
    init(pos * 2 + 1, mid + 1, r);
    result lres = info[pos * 2], rres = info[pos * 2 + 1];
    MergeString
    info[pos] = res_info;
}
void update(int pos, int l, int r, int ul, int ur, int a) {
    if (l > ur || r < ul) return;
    if (l >= ul && r <= ur) {
        apply(pos, a); return;
    }
    int mid = l + r >> 1; pushdown(pos);
    update(pos * 2, l, mid, ul, ur, a);
    update(pos * 2 + 1, mid + 1, r, ul, ur, a);
    result lres = info[pos * 2], rres = info[pos * 2 + 1];
    MergeString
    info[pos] = res_info;
}
result query_res;
bool is_empty;
void query(int pos, int l, int r, int ul, int ur) {
    if (l > ur || r < ul) return;
    if (l >= ul && r <= ur) {
        if (is_empty) {
            is_empty = false; query_res = info[pos];
        } else {
            result lres = query_res, rres = info[pos];
            MergeString
            query_res = res_info;
        }
    }
    int mid = l + r >> 1; pushdown(pos);
    query(pos * 2, l, mid, ul, ur);
    query(pos * 2 + 1, mid + 1, r, ul, ur);
}
TagReferenceProgram
QueryReferenceProgram
int main() {
    for (int i = 1; i <= n; ++i) {
        w[i] = nondet_int();
        __CPROVER_assume(KIntMin <= w[i] && w[i] <= KIntMax);
    }
    init(1, 1, n);
    for (int _ = 1; _ <= m; ++_) {
        int type = nondet_int(), l = nondet_int(), r = nondet_int();
        __CPROVER_assume(0 <= type && type <= 1 && 1 <= l && l <= r && r <= n);
        if (type == 0) {
            int a = nondet_int();
            __CPROVER_assume(KIntMin <= a && a <= KIntMax);
            update(1, 1, n, l, r, a);
            for (int i = l; i <= r; ++i) {
                mod(i, a);
            }
        } else {
            int ref = oracle(l, r);
            is_empty = true;
            query(1, 1, n, l, r);
            assert(ref == query_res.x0);
        }
    }
}
