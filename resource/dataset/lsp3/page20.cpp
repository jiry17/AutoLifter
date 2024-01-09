// ReferenceProgram
int oracle() {
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        int ma = -KINF;
        for (int j = i; j <= n; ++j) {
            ma = max(ma, w[j]);
            if (ma < j - i + 1) ans = max(j - i + 1, ans);
        }
    }
    return ans;
}