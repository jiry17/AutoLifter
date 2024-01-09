#include <cassert>
#include <cstdio>
#include <sys/time.h>
double difftime(timeval start, timeval end) {
    long long second_diff = end.tv_sec - start.tv_sec;
    if (second_diff < 0) second_diff += 24 * 3600;
    long long u_diff = end.tv_usec - start.tv_usec;
    return second_diff + u_diff * 1e-6;
}
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a < b ? b : a;
}
const int N=10000000;
int n, w[N + 10];
struct result {
    int x0,x1;
};

int lsp() {
    int res = 0, l = 1;
    result info = (result){1,0};
    for (int r = 1; r <= n; ++r) {
        int a = w[r];
        info = (result){(((a)+(info.x1))<=(10))?(1):(0),(a)+(info.x1)};
        while (l <= r && !info.x0) {
            a = w[l];
            info = (result){((info.x1)<=((10)+(a)))?(1):(0),((a)*(-1))+(info.x1)};
            l += 1;
        }
        res = max(res, r - l + 1);
    }
    return res;
}

int main() {
    scanf("%d\n",&n);
    for (int i = 1; i <= n; ++i) scanf("%d", &w[i]);
    timeval start;
    gettimeofday(&start, NULL);
    result res;
    int myres = lsp();
    timeval end;
    gettimeofday(&end, NULL);
    double cost = difftime(start, end);
    printf("%d\n", myres);
    printf("%.10lf\n", cost);
}