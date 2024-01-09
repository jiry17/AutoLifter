// ReferenceProgram
int oracle() {
    int ans = -KINF;
    for (int i = 1; i <= n; ++i) {
        int current = 1;
        for (int j = i; j <= n; ++j) {
            current *= w[j];
            ans = max(ans, current);
        }
    }
    return ans;
}
