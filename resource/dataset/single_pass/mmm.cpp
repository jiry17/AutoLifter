// ReferenceProgram
int oracle() {
    int p = 0, np = 0, z = 0;
    for (int i = 1; i <= n; ++i) {
        int prep = p, prez = z, prenp = np;
        p = max(prez, prenp) + w[i];
        np = max(prez, prep) - w[i];
        z = max(prenp, prep);
    }
    return max(max(p, np), z);
}