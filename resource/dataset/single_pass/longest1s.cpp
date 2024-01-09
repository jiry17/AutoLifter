// ReferenceProgram
int oracle() {
    int ml = 0, len = 0;
    for (int i = 1; i <= n; ++i) {
        len = w[i] == 1 ? len + 1 : 0;
        ml = max(ml, len);
    }
    return ml;
}