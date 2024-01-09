// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = -w[pos];
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int mi = KINF, second_mi = KINF;
    for (int i = l; i <= r; ++i) {
        if (w[i] < mi) {
            second_mi = mi; mi = w[i];
        } else second_mi = min(second_mi, w[i]);
    }
    return second_mi;
}