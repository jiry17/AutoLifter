// ReferenceProgram
int oracle() {
    bool s0 = false, s1 = false;
    int result = 0;
    for (int i = 2; i <= n; ++i) {
        if (w[i] == 0) result += w[i - 1];
    }
    return result;
}