// ReferenceProgram
int oracle() {
    int cnt = 0;
    for (int i = 1; i <= n; ++i) {
        cnt += w[i];
        if (cnt < 0) return 0;
    }
    return 1;
}