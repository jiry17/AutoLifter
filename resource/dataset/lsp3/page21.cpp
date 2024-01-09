// ReferenceProgram
int oracle() {
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        int ma = -KINF;
        for (int j = i; j <= n; ++j) {
            if (w[j] < w[i]) break;
            ma = max(ma, w[j]);
            if (ma == w[j]) ans = max(ans, j - i + 1);
        }
    }
    return ans;
}