// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = -w[pos];
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int res = KINF;
    for (int i = l; i <= r; ++i) {
        res = min(res, w[i]);
    }
    return res;
}