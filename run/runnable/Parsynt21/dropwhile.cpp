#include <cstdio>
#include <sys/time.h>
#include <omp.h>
#include <algorithm>
const int N=100000000;
int w[N + 10];
int n, lim;
double difftime(timeval start, timeval end) {
    long long second_diff = end.tv_sec - start.tv_sec;
    if (second_diff < 0) second_diff += 24 * 3600;
    long long u_diff = end.tv_usec - start.tv_usec;
    return second_diff + u_diff * 1e-6;
}
struct result {
    int result1 = 0;
    int len = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.result1 = (x > 0) ? 1 : 0;
        _s1.len = _s1.len + 1;
        return _s1;
    } else {
        result $L5, $R5;
        int mid = l + r >> 1;
#pragma omp task shared($L5) if(r - l > lim)
        $L5 = dac(l, mid);
#pragma omp task shared($R5) if(r - l > lim)
        $R5 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.result1 = ($L5.result1 > 0 ) ? $L5.result1 : 
            (($R5.result1 > 0) ? ($L5.len + $R5.result1) : 0);
        _s1.len = $L5.len + $R5.len;
        return _s1;
    }
}
int main(){
    scanf("%d",&n);
    for (int i = 1; i <= n; ++i) {
        scanf("%d",&w[i]);
    }
    lim = n / 100;
    timeval start;
    gettimeofday(&start, NULL);
    result res;
    omp_set_num_threads(8);
#pragma omp parallel
    {
#pragma omp single
        {
            printf("thread num: %d\n", omp_get_num_threads());
            res = dac(1, n);
        }
    }
    timeval end;
    gettimeofday(&end, NULL);
    double cost = difftime(start, end);
    printf("%d\n", res.result1);
    printf("%.10lf\n", cost);
}