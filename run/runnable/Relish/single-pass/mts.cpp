#include <cstdio>
#include <sys/time.h>
const int N=100000000;
int w[N + 10];
int n;
double difftime(timeval start, timeval end) {
    long long second_diff = end.tv_sec - start.tv_sec;
    if (second_diff < 0) second_diff += 24 * 3600;
    long long u_diff = end.tv_usec - start.tv_usec;
    return second_diff + u_diff * 1e-6;
}
struct result {
    int x0;
};
int main(){
    scanf("%d",&n);
    for (int i = 1; i <= n; ++i) {
        scanf("%d",&w[i]);
    }
    timeval start;
    gettimeofday(&start, NULL);
    int a = w[1];
    result res = (result){((0)<=(a))?(a):(0)};
    for (int i = 2; i <= n; ++i) {
        a = w[i];
        res = (result){((res.x0)<((0)-(a)))?(0):((res.x0)+(a))};
    }
    timeval end;
    gettimeofday(&end, NULL);
    double cost = difftime(start, end);
    printf("%d\n", res.x0);
    printf("%.10lf\n", cost);
}