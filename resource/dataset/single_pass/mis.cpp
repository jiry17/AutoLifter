// ReferenceProgram
int oracle() {
    int p = 0, np = 0;
    for (int i = 1; i <= n; ++i) {
        int prep = p;
        p = np + w[i];
        np = max(prep, np);
    }
    return max(p, np);
}