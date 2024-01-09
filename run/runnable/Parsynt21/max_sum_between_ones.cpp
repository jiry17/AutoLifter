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
    int ms = 0; int cs = 0; bool ms_aux4 = true; int ms_aux5 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.ms = std::max(_s1.ms, (!(x == 0) ? _s1.cs + x : 0));
        _s1.cs = (!(x == 0) ? _s1.cs + x : 0);
        _s1.ms_aux4 = _s1.ms_aux4 && (!(x == 0));
        _s1.ms_aux5 = (_s1.ms_aux4 && (!(x == 0)) ? _s1.ms_aux5 + x : _s1.ms_aux5);
        return _s1;
    } else {
        result $L4, $R4;
        int mid = l + r >> 1;
#pragma omp task shared($L4) if(r - l > lim)
        $L4 = dac(l, mid);
#pragma omp task shared($R4) if(r - l > lim)
        $R4 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.ms = std::max($L4.ms, (std::max(($L4.cs + $R4.ms_aux5), $R4.ms)));
        _s1.cs = $R4.ms_aux4 ? $L4.cs + $R4.cs : $R4.cs;
        _s1.ms_aux4 = $L4.ms_aux4 && $R4.ms_aux4;
        _s1.ms_aux5 = $L4.ms_aux4 && (!(-1 == $L4.ms)) ? $L4.ms + $R4.ms_aux5 : $L4.ms_aux5;
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
    printf("%d\n", res.ms);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/max_sum_between_ones.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { ms = 0; cs = 0; ms_aux4 = true; ms_aux5 = 0; }for(x in a){
//                                                         _s1 = {
//                                                           ms =
//                                                           max (_s1.ms,
//                                                             (if ¬ (x = 0) then
//                                                              _s1.cs + x else 0));
//                                                           cs =
//                                                           if ¬ (x = 0) then
//                                                             _s1.cs + x else 0;
//                                                           ms_aux4 =
//                                                           _s1.ms_aux4 &&
//                                                             (¬ (x = 0));
//                                                           ms_aux5 =
//                                                           if
//                                                             _s1.ms_aux4 &&
//                                                               (¬ (x = 0))
//                                                             then
//                                                             _s1.ms_aux5 + x
//                                                             else _s1.ms_aux5;
//                                                         }
//                                                         }
//                                                         return _s1;
// //--- Splitting predicate: a(0) = 0
// //--- Join function ---
// join($L4,$R4) = {
//   ms = max ($L4.ms, (max (($L4.cs + $R4.ms_aux5), $R4.ms)));
//   cs = if $R4.ms_aux4 then $L4.cs + $R4.cs else $R4.cs;
//   ms_aux4 = $L4.ms_aux4 && $R4.ms_aux4;
//   ms_aux5 =
//   if $L4.ms_aux4 && (¬ (-1 = $L4.ms)) then $L4.ms + $R4.ms_aux5 else
//     $L4.ms_aux5;
// }
// // Synthesized in (predicate : 0.48 s) + (join : 18.54 s)