// ReferenceProgram
int oracle() {
    int sum = 0, mps = 0;
    for (int i = 1; i <= n; ++i) {
        sum += w[i];
        mps = max(mps, sum);
    }
    return mps;
}