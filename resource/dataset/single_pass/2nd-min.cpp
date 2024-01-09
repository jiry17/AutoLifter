// ReferenceProgram
int oracle() {
    int min_value = KINF, second_min_value = KINF;
    for (int i = 1; i <= n; ++i) {
        if (w[i] < min_value) {
            second_min_value = min_value;
            min_value = w[i];
        } else second_min_value = min(second_min_value, w[i]);
    }
    return second_min_value;
}