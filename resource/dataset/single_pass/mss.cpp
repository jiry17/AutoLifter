// ReferenceProgram
int oracle() {
    int mss = 0, mts = 0;
    for (int i = 1; i <= n; ++i) {
        mts = max(mts + w[i], 0);
        mss = max(mss, mts);
    }
    return mss;
}