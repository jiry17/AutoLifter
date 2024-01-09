#include <cstdio>
#include <sys/time.h>
#include <omp.h>
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
    bool seen1 = false;
    bool res = false;
    bool res_aux1 = false;
};
result dac(int l, int r)
{
    if (l == r) {
        int a = w[l];
        return result{false || a, (!(! false) || a) || false, (! a) || false};
    } else {
        result $L5, $R5;
        int mid = l + r >> 1;
#pragma omp task shared($L5) if(r - l > lim)
        $L5 = dac(l, mid);
#pragma omp task shared($R5) if(r - l > lim)
        $R5 = dac(mid + 1, r);
#pragma omp taskwait
        bool new_seen1 = $L5.seen1 || $R5.seen1;
        bool new_res = ($R5.res_aux1) ? $R5.res || $L5.seen1 : ($L5.seen1 && (! $R5.seen1)) || $L5.res;
        bool new_res_aux1 = $L5.res_aux1 || $R5.res_aux1;
        return result{new_seen1, new_res, new_res_aux1};
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
    printf("%d\n", res.res);
    printf("%.10lf\n", cost);
}


//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/0after1.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { seen1 = false; res = false; res_aux1 = false; }for(x in a){
//                                                          _s1 = {
//                                                            seen1 =
//                                                            _s1.seen1 || x;
//                                                            res =
//                                                            (¬
//                                                             (¬ _s1.seen1) || x)
//                                                              || _s1.res;
//                                                            res_aux1 =
//                                                            (¬ x) ||
//                                                              _s1.res_aux1;
//                                                          }
//                                                          }
//                                                          return _s1;
// //--- Splitting predicate: (empty)
// //--- Join function ---
// join($L5,$R5) = {
//   seen1 = $L5.seen1 || $R5.seen1;
//   res =
//   if $R5.res_aux1 then $R5.res || $L5.seen1 else
//     ($L5.seen1 && (¬ $R5.seen1)) || $L5.res;
//   res_aux1 = $L5.res_aux1 || $R5.res_aux1;
// }
// // Synthesized in (predicate : 0.02 s) + (join : 4.50 s)                                      