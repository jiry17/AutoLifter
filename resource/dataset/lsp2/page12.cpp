// Reference Program
int oracle() {
    int res = 0;
    for (int i = 1; i <= n; ++i) {
        int sum = 0;
        for (int j = i; j <= n; ++j) {
            sum += w[j];
            if (sum > 10) break;
            else res = max(res, j - i + 1);
        }
    }
    return res;
}