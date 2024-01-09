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
    int sum = 0;
    int len = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.sum = _s1.sum + x;
        _s1.len = _s1.len + 1;
        return _s1;
    } else {
        result $L, $R;
        int mid = l + r >> 1;
#pragma omp task shared($L) if(r - l > lim)
        $L = dac(l, mid);
#pragma omp task shared($R) if(r - l > lim)
        $R = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.sum = $L.sum + $R.sum;
        _s1.len = $L.len + $R.len;
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
    printf("%d\n", res.sum / res.len);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/average.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { sum = 0; len = 0; }for(x in a){
//                              _s1 = {
//                                sum = _s1.sum + x;
//                                len = _s1.len + 1;
//                              }
//                              }
//                              return _s1;
// //--- Splitting predicate: (empty)
// //--- Join function ---
// join($L,$R) = { sum = $L.sum + $R.sum;
//                 len = $L.len + $R.len;
// }
// // Synthesized in (predicate : 0.02 s) + (join : 0.00 s)

//  SUCCESS  Finished.