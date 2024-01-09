// ReferenceProgram
int oracle() {
    int current = 1, ans = -KINF;
    for (int i = 1; i <= n; ++i) {
        current *= w[i];
        ans = max(ans, current);
    }
    return ans;
}