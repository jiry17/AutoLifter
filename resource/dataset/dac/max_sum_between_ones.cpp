// ReferenceProgram
int oracle() {
    int ms = 0, cs = 0;
    for (int i = 1; i <= n; ++i) {
        cs = w[i] != 1 ? cs + w[i] : 0;
        ms = max(ms, cs);
    }
    return ms;
}