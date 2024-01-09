// ReferenceProgram
int oracle() {
    int f = 0, cnt = 0;
    for (int i = 1; i <= n; ++i) {
        cnt += (w[i] && !f) ? 1 : 0;
        f = w[i];
    }
    return cnt;
}