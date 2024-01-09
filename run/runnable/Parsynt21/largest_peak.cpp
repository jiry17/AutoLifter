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
    int cmo = 0;
    int lpeak = 0;
    int lpeak_aux2 = 0;
    bool lpeak_aux3 = true;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.cmo = (x > 0) ? (_s1.cmo + x) : 0;
        _s1.lpeak = std::max(_s1.lpeak, ((x > 0) ? _s1.cmo + x : 0));
        _s1.lpeak_aux2 = (_s1.lpeak_aux3 && (x > 0)) ? _s1.lpeak_aux2 + x : _s1.lpeak_aux2;
        _s1.lpeak_aux3 = _s1.lpeak_aux3 && (x > 0);
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
        _s1.cmo = $R4.lpeak_aux3 ?
            ($R4.lpeak_aux3 ? ($L4.cmo + $R4.lpeak_aux2) : $L4.lpeak) : $R4.cmo;
        _s1.lpeak = std::max($L4.lpeak, (std::max(($L4.cmo + $R4.lpeak_aux2), $R4.lpeak)));
        _s1.lpeak_aux2 =
            $L4.lpeak_aux3 ?
            (($L4.lpeak_aux3 && ($R4.lpeak_aux2 > -1)) ?
            ($L4.lpeak_aux2 + $R4.lpeak_aux2) : 1)
            :
            (($L4.lpeak_aux3 && ($L4.lpeak_aux2 > $L4.lpeak)) ? -2 :
            $L4.lpeak_aux2);
        _s1.lpeak_aux3 = $L4.lpeak_aux3 && $R4.lpeak_aux3;
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
    printf("%d\n", res.cmo);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/largest_peak.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { cmo = 0; lpeak = 0; lpeak_aux2 = 0; lpeak_aux3 = true; }for(x in a){
//                                                                   _s1 = {
//                                                                     cmo =
//                                                                     if 
//                                                                     x > 0 then
//                                                                     _s1.cmo + x
//                                                                     else 0;
//                                                                     lpeak =
//                                                                     max
//                                                                     (_s1.lpeak,
//                                                                     (if 
//                                                                     x > 0 then
//                                                                     _s1.cmo + x
//                                                                     else 0));
//                                                                     lpeak_aux2 =
//                                                                     if
//                                                                     _s1.lpeak_aux3
//                                                                     && 
//                                                                     (x > 0)
//                                                                     then
//                                                                     _s1.lpeak_aux2
//                                                                     + x else
//                                                                     _s1.lpeak_aux2;
//                                                                     lpeak_aux3 =
//                                                                     _s1.lpeak_aux3
//                                                                     && 
//                                                                     (x > 0);
//                                                                   }
//                                                                   }
//                                                                   return _s1;
// //--- Splitting predicate: a(0) <= 0
// //--- Join function ---
// join($L4,$R4) = {
//   cmo =
//   if $R4.lpeak_aux3 then
//     if $R4.lpeak_aux3 then $L4.cmo + $R4.lpeak_aux2 else $L4.lpeak else $R4.cmo;
//   lpeak = max ($L4.lpeak, (max (($L4.cmo + $R4.lpeak_aux2), $R4.lpeak)));
//   lpeak_aux2 =
//   if $L4.lpeak_aux3 then
//     if $L4.lpeak_aux3 && ($R4.lpeak_aux2 > -1) then
//       $L4.lpeak_aux2 + $R4.lpeak_aux2 else 1
//     else
//     if $L4.lpeak_aux3 && ($L4.lpeak_aux2 > $L4.lpeak) then -2 else
//       $L4.lpeak_aux2;
//   lpeak_aux3 = $L4.lpeak_aux3 && $R4.lpeak_aux3;
// }
// // Synthesized in (predicate : 0.39 s) + (join : 17.99 s)