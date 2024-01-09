// ReferenceProgram
int oracle() {
    int sum = 0;
    for (int i = 1; i <= n; ++i) sum += w[i];
    return sum / n;
}