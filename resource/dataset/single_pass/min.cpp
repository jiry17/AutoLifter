// ReferenceProgram
int oracle() {
    int ans = KINF;
    for (int i = 1; i <= n; i++) {
        ans = min(ans, w[i]);
    }
    return ans;
}