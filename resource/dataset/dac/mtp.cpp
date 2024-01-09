// ReferenceProgram
int oracle() {
    int current = 1, ans = -KINF;
    for (int i = n; i >= 1; --i) {
        current *= w[i];
        ans = max(ans, current);
    }
    return ans;
}