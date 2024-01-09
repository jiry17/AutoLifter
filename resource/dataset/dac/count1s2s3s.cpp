// ReferenceProgram
int oracle() {
    bool s1 = false, s2 = false;
    int result = 0;
    for (int i = 1; i <= n; ++i) {
        result += ((w[i] == 3 && (s2 || s1)) ? 1 : 0);
        s2 = (w[i] == 2) && (s1 || s2);
        s1 = (w[i] == 1);
    }
    return result;
}