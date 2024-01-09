// ReferenceProgram
int oracle() {
    int cl = 0, ml = 0;
    for (int i = 2; i <= n; ++i) {
        cl = w[i - 1] < w[i] ? cl + 1 : 0;
        ml = max(ml, cl);
    }
    return ml;
}