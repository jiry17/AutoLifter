// ReferenceProgram
int oracle() {
    int res = 1;
    for (int i = 1; i <= n; i++) {
        for (int j = i + 1; j <= n; ++j) {
            if (w[j - 1] < w[j]) res = max(res, j - i + 1);
            else break;
        }
    }
    return res;
}