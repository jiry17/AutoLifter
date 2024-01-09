// ReferenceProgram
int oracle() {
    int pre = 1, result = 0, pos = 0;
    for (int i = 1; i <= n + 1; ++i) {
        if (i == n + 1 || w[i] == 0) {
            int current_size = i - pre;
            if (current_size > result) {
                result = current_size;
                pos = pre - 1;
            }
            pre = i + 1;
        }
    }
    return pos;
}