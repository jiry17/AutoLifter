// ReferenceProgram
int oracle() {
    int md = 0, cd = 0;
    for (int i = 1; i <= n; ++i) {
        cd = w[i] ? cd + 1 : 0;
        md = max(md, cd);
    }
    return md;
}