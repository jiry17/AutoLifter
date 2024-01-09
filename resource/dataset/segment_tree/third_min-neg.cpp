// TagReferenceProgram
void mod(int pos, int a) {
    w[pos] = -w[pos];
}
// QueryReferenceProgram
int oracle(int l, int r) {
    int min_value = KINF;
    int second_min_value = KINF;
    int third_min_value = KINF;
    for (int i = l; i <= r; ++i) {
        if (w[i] < min_value) {
            third_min_value = second_min_value;
            second_min_value = min_value;
            min_value = w[i];
        } else if (w[i] < second_min_value) {
            third_min_value = second_min_value;
            second_min_value = w[i];
        } else third_min_value = min(third_min_value, w[i]);
    }
    return third_min_value;
}