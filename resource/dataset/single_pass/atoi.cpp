// ReferenceProgram
int oracle() {
    int result = 0;
    for (int i = 1; i <= n; ++i) {
        result = result * 10 + w[i];
    }
    return result;
}