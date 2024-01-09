// ReferenceProgram
int oracle() {
    int ma = 0, vis = 1;
    for (int i = 1; i <= n; i++) {
        vis = (ma <= w[i]);
        ma = max(ma, w[i]);
    }
    return vis;
}