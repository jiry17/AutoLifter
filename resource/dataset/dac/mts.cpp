// ReferenceProgram
int oracle() {
    int mts = 0;
    for (int i = 1; i <= n; ++i) {
        mts = max(mts + w[i], 0);
    }
    return mts;
}