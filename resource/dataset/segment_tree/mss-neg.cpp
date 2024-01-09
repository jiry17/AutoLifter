// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = -w[pos];
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int mss = 0, mts = 0;
    for (int i = l; i <= r; ++i) {
        mts = max(mts + w[i], 0);
        mss = max(mss, mts);
    }
    return mss;
}