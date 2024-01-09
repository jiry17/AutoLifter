// ReferenceProgram
int oracle() {
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        int mi = KINF, ma = -KINF;
        for (int j = i; j <= n; ++j) {
            mi = min(mi, w[j]);
            ma = max(ma, w[j]);
            if (mi + ma > j - i + 1) ans = max(ans, j - i + 1);
        }
    }
    return ans;
}