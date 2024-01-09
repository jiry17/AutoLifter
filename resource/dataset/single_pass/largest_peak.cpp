// ReferenceProgram
int oracle() {
    int cmo = 0, lpeak = 0;
    for (int i = 1; i <= n; ++i) {
        cmo = w[i] > 0 ? cmo + w[i] : 0;
        lpeak = max(cmo, lpeak);
    }
    return lpeak;
}