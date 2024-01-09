// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = a;
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int pre = 0, result = 0, pos = 0;
    for (int i = 0; i <= r - l + 1; ++i) {
        if (i == r - l + 1 || w[i + l] == 0) {
            int current_size = i - pre;
            if (current_size > result) {
                result = current_size;
                pos = pre;
            }
            pre = i + 1;
        }
    }
}