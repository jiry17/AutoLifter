// ReferenceProgram
int oracle() {
    int mps = 0, pos = 0, sum = 0;
    for (int i = 1; i <= n; ++i) {
        sum += w[i];
        if (sum > mps) pos = i - 1;
        mps = max(mps, sum);
    }
    return pos;
}