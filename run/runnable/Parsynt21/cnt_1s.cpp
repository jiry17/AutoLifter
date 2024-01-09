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
    int f = 0;
    int cnt = 0;
    bool f_aux = true;
    int cnt_aux5 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.f = x;
        _s1.cnt = _s1.cnt + (x && (!_s1.f) ? 1 : 0);
        _s1.f_aux = x;
        _s1.cnt_aux5 = _s1.cnt_aux5 + (x && (!_s1.f_aux) ? 1 : 0);
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
        _s1.f = $R3.f;
        _s1.cnt = $L3.cnt + ($L3.f_aux ? $R3.cnt : $R3.cnt_aux5);
        _s1.f_aux = $R3.f_aux;
        _s1.cnt_aux5 =
        $L3.f_aux ? $L3.cnt_aux5 + ($L3.f ? $R3.cnt : -1) :
            $R3.cnt_aux5 + ($L3.f_aux ? $L3.cnt : $L3.cnt_aux5);
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
    printf("%d\n", res.cnt);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/cnt_1s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { f = 0; cnt = 0; f_aux = ¬ 0; cnt_aux5 = 0; }for(x in a){
//                                                        _s1 = {
//                                                          f = x;
//                                                          cnt =
//                                                          _s1.cnt +
//                                                            (if x && (¬ _s1.f)
//                                                             then 1 else 0);
//                                                          f_aux = x;
//                                                          cnt_aux5 =
//                                                          _s1.cnt_aux5 +
//                                                            (if
//                                                             x && (¬ _s1.f_aux)
//                                                             then 1 else 0);
//                                                        }
//                                                        }
//                                                        return _s1;
// //--- Splitting predicate: ¬ (a(0) && (¬ a(-1)))
// //--- Join function ---
// join($L3,$R3) = {
//   f = $R3.f;
//   cnt = $L3.cnt + (if $L3.f_aux then $R3.cnt else $R3.cnt_aux5);
//   f_aux = $R3.f_aux;
//   cnt_aux5 =
//   if $L3.f_aux then $L3.cnt_aux5 + (if $L3.f then $R3.cnt else -1) else
//     $R3.cnt_aux5 + (if $L3.f_aux then $L3.cnt else $L3.cnt_aux5);
// }
// // Synthesized in (predicate : 0.13 s) + (join : 9.33 s)