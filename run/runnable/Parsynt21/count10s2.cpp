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
    bool s0 = false;
    int count = 0;
    bool s0_aux = true;
    int count_aux5 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.s0 = (x == 1) || ((x == 0) && _s1.s0);
        _s1.count = _s1.count + ((_s1.s0 && (x == 2)) ? 1 : 0);
        _s1.s0_aux = (x == 1) || ((x == 0) && _s1.s0_aux);
        _s1.count_aux5 = _s1.count_aux5 + ((_s1.s0_aux && (x == 2)) ? 1 : 0);
        return _s1;
    } else {
        result $L3, $R3;
        int mid = l + r >> 1;
#pragma omp task shared($L3) if(r - l > lim)
        $L3 = dac(l, mid);
#pragma omp task shared($R3) if(r - l > lim)
        $R3 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.s0 = $R3.s0_aux ? ($L3.s0 || $R3.s0) : ($R3.s0 && $L3.s0_aux);
        _s1.count =
            $L3.s0 ? ($L3.count + ($L3.s0 ? $R3.count_aux5 : $L3.count))
            : ($L3.count + $R3.count);
        _s1.s0_aux = $L3.s0_aux ? $R3.s0_aux : ($L3.s0_aux || $R3.s0);
        _s1.count_aux5 =
            $L3.s0_aux ? ($L3.count_aux5 + $R3.count_aux5) :
            ($R3.count + $L3.count_aux5);
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
    printf("%d\n", res.s0);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/count10s2.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { s0 = false; count = 0; s0_aux = true; count_aux5 = 0; }for(x in a){
//                                                                  _s1 = {
//                                                                    s0 =
//                                                                    (x = 1) ||
//                                                                     ((x = 0) &&
//                                                                     _s1.s0);
//                                                                    count =
//                                                                    _s1.count +
//                                                                     (if
//                                                                     _s1.s0 &&
//                                                                     (x = 2)
//                                                                     then 1 else
//                                                                     0);
//                                                                    s0_aux =
//                                                                    (x = 1) ||
//                                                                     ((x = 0) &&
//                                                                     _s1.s0_aux);
//                                                                    count_aux5 =
//                                                                    _s1.count_aux5
//                                                                     +
//                                                                     (if
//                                                                     _s1.s0_aux
//                                                                     && 
//                                                                     (x = 2)
//                                                                     then 1 else
//                                                                     0);
//                                                                  }
//                                                                  }
//                                                                  return _s1;
// //--- Splitting predicate: ¬ ((¬ (a(-1) = 1)) && (a(0) = 2))
// //--- Join function ---
// join($L3,$R3) = {
//   s0 = if $R3.s0_aux then $L3.s0 || $R3.s0 else $R3.s0 && $L3.s0_aux;
//   count =
//   if $L3.s0 then $L3.count + (if $L3.s0 then $R3.count_aux5 else $L3.count)
//     else $L3.count + $R3.count;
//   s0_aux = if $L3.s0_aux then $R3.s0_aux else $L3.s0_aux || $R3.s0;
//   count_aux5 =
//   if $L3.s0_aux then $L3.count_aux5 + $R3.count_aux5 else
//     $R3.count + $L3.count_aux5;
// }
// // Synthesized in (predicate : 0.13 s) + (join : 12.23 s)