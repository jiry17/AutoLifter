// ReferenceProgram
int oracle() {
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        ans += w[i];
    }
    return ans;
}