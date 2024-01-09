// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = a;
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int mts = 0;
    for (int i = l; i <= r; ++i) {
        mts = max(mts, 0) + w[i];
    }
    return mts;
}