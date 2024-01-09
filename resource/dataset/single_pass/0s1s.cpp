// ReferenceProgram
int oracle() {
    int an = 1, bn = 1;
    for (int i = 1; i <= n; ++i) {
        an = w[i] && an;
        bn = ((!w[i]) || an) && bn;
    }
    return bn;
}