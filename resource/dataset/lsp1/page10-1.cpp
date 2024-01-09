// ReferenceProgram
int oracle() {
    int ans = 1;
    for (int i = 1; i <= n; ++i) {
        int mi = w[i];
        for (int j = i + 1; j <= n; ++j) {
            if (w[j] - w[j - 1] >= 2) break;
            mi = min(mi, w[j]);
            if (mi == w[i]) ans = max(ans, j - i + 1);
        }
    }
    return ans;
}