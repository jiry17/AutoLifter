// ReferenceProgram
int oracle() {
    int res = 0, p = 0, n = 0;
    for (int i = 1; i <= n; ++i) {
        int prep = p;
        p = max(n, 0) + w[i];
        n = max(prep, 0) - w[i];
        res = max(res, max(p, n));
    }
    return res;
}