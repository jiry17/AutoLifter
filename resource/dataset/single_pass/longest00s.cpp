// ReferenceProgram
int oracle() {
    int cl = 0, ml = 0, ml_tmp = 0;
    for (int i = 1; i <= n; ++i) {
        cl = w[i] == 0 ? cl + 1 : 0;
        ml_tmp = max(ml_tmp, cl);
        if ((ml_tmp & 1) == 0) ml = ml_tmp;
    }
    return ml;
}