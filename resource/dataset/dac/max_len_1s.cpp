// ReferenceProgram
int oracle() {
    int ma = 0, l = 0;
    for (int i = 1; i <= n; ++i) {
        if (w[i] == 0) {
            ma = max(ma, l); l = 0;
        } else ++l;
    }
    return max(ma, l);
}