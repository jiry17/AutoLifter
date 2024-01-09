// ReferenceProgram
int oracle() {
    int pos = -1, mts = 0;
    for (int i = 1; i <= n; ++i) {
        if (mts + w[i] < 0) pos = i - 1;
        mts = max(0, mts + w[i]);
    }
    return pos;
}