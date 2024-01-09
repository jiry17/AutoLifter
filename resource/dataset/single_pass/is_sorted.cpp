// ReferenceProgram
int oracle() {
    int result = 1, pre = -KINF;
    for (int i = 1; i <= n; ++i) {
        if (pre >= w[i]) result = 0;
        pre = w[i];
    }
    return result;
}