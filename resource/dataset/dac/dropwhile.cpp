// ReferenceProgram
int oracle() {
    for (int i = 1; i <= n; i++) {
        if (w[i]) return i - 1;
    }
    return n;
}