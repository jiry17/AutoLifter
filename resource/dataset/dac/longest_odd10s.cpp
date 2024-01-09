// ReferenceProgram
int oracle() {
    bool s1 = false, s2 = false;
    int cl = 0, ml = 0;
    for (int i = 1; i <= n; ++i) {
        s1 = (s2 && w[i] == 1);
        s2 = (w[i] == 0);
        cl = (s1 ? cl + 1 : (s2 ? cl : 0));
        ml = (cl % 2 == 1 ? max(ml, cl) : ml);
    }
    return ml;
}