// ReferenceProgram
int oracle() {
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        int mi = KINF;
        for (int j = i; j <= n; ++j) {
            mi = min(mi, w[j]);
            if (mi == w[i]) ans = max(ans, j - i + 1);
        }
    }
    return ans;
}