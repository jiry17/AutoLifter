// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = a;
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int res = 0;
    for (int i = l; i <= r; ++i) res += w[i] * w[i];
    return res;
}