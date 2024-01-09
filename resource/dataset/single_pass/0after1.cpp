// ReferenceProgram
int oracle() {
    bool seen1 = false, res = false;
    for (int i = 1; i <= n; ++i) {
        if (seen1 && !(w[i])) res = true;
        seen1 = seen1 || w[i];
    }
    return res;
}