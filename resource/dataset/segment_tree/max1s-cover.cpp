// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = a;
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int ml = 0, len = 0;
    for (int i = l; i <= r; ++i) {
        len = w[i] == 1 ? len + 1 : 0;
        ml = max(ml, len);
    }
    return ml;
}