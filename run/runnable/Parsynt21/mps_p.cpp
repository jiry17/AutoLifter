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
    int mps = 0; int pos = 0; int sum = 0; int i = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.mps = std::max(_s1.mps, (_s1.sum + x));
        _s1.pos = ((_s1.sum + x) > _s1.mps) ? _s1.i : _s1.pos;
        _s1.sum = _s1.sum + x;
        _s1.i = _s1.i + 1; 
        return _s1;
    } else {
        result $L1, $R1;
        int mid = l + r >> 1;
#pragma omp task shared($L1) if(r - l > lim)
        $L1 = dac(l, mid);
#pragma omp task shared($R1) if(r - l > lim)
        $R1 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.mps = std::max($L1.mps, ($L1.sum + $R1.mps));
        _s1.pos = ((std::max(0, $L1.mps)) + 1) > (std::max($L1.mps, ($L1.sum + $R1.mps))) ?
            std::max(0, $L1.pos) : $R1.pos + $L1.i;
        _s1.sum = $L1.sum + $R1.sum;
        _s1.i = $L1.i + $R1.i;
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
    printf("%d\n", res.mps);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/mps_p.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { mps = 0; pos = 0; sum = 0; i = 0; }for(x in a){
//                                              _s1 = {
//                                                mps =
//                                                max (_s1.mps, (_s1.sum + x));
//                                                pos =
//                                                if (_s1.sum + x) > _s1.mps then
//                                                  _s1.i else _s1.pos;
//                                                sum = _s1.sum + x;
//                                                i = _s1.i + 1;
//                                              }
//                                              }
//                                              return _s1;
// //--- Splitting predicate: (a(-1) + a(0)) > (max (0, (a(-1) + a(-1))))
// //--- Join function ---
// join($L1,$R1) = {
//   mps = max ($L1.mps, ($L1.sum + $R1.mps));
//   pos =
//   if ((max (0, $L1.mps)) + 1) > (max ($L1.mps, ($L1.sum + $R1.mps))) then
//     max (0, $L1.pos) else $R1.pos + $L1.i;
//   sum = $L1.sum + $R1.sum;
//   i = $L1.i + $R1.i;
// }
// // Synthesized in (predicate : 0.10 s) + (join : 19.87 s)