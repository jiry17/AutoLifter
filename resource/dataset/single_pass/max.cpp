// ReferenceProgram
int oracle() {
    int ans = -KINF;
    for (int i = 1; i <= n; i++) ans = max(ans, w[i]);
    return ans;
}