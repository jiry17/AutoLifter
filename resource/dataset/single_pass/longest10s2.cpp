// ReferenceProgram
int oracle() {
    bool s0 = false, s1 = false;
    int ml = 0, len = 0;
    for (int i = 1; i <= n; ++i) {
        s1 = s0 && (w[i] == 2);
        s0 = (w[i] == 1) || (w[i] == 0 && s0);
        len = (s1 || s0) ? len + 1 : 0;
        ml = s1 ? max(ml, len) : ml;
        if (s1) len = 0;
        if (w[i] == 1) len = 1;
    }
    return ml;
}