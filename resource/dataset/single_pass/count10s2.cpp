// ReferenceProgram
int oracle() {
    bool s0 = false;
    int c = 0;
    for (int i = 1; i <= n; ++i) {
        c += s0 && (w[i] == 2 ? 1 : 0);
        s0 = (w[i] == 1) || (s0 && w[i] == 0);
    }
    return c;
}