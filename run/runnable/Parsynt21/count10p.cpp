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
    bool s1 = false;
    int scount = 0;
    bool s1_aux = true;
    int scount_aux5 = 0;
    bool s0_aux = true;
    bool s1_aux1 = false;
    int scount_aux6 = 0;
    bool s1_aux2 = true;
    int scount_aux7 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.s0 = (x == 1);
        _s1.s1 = (x == 0) && _s1.s0 || _s1.s1;
        _s1.scount = _s1.scount + (_s1.s1 && (! (x = 0)) ? 1 : 0);
        _s1.s1_aux = (x == 0) && _s1.s0 || _s1.s1_aux;
        _s1.scount_aux5 =
            _s1.scount_aux5 + (_s1.s1_aux && (! (x == 0)) ? 1 : 0);
        _s1.s0_aux = (x == 1);
        _s1.s1_aux1 = (x == 0) && _s1.s0_aux || _s1.s1_aux1;
        _s1.scount_aux6 =
            _s1.scount_aux6 + (_s1.s1_aux1 && (! (x == 0)) ? 1 : 0);
        _s1.s1_aux2 = (x == 0) && _s1.s0_aux || _s1.s1_aux2;
        _s1.scount_aux7 =
            _s1.scount_aux7 + (_s1.s1_aux2 && (! (x == 0)) ? 1 : 0);
        return _s1;
    } else {
        result $L6, $R6;
        int mid = l + r >> 1;
#pragma omp task shared($L6) if(r - l > lim)
        $L6 = dac(l, mid);
#pragma omp task shared($R6) if(r - l > lim)
        $R6 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.s0 = $R6.s0;
        _s1.s1 = ($L6.s1 || $L6.s0) ? $R6.s1_aux1 : $R6.s1;
        _s1.scount =
            $L6.s1 ? ($L6.s1_aux ? $R6.scount_aux7 : -1) :
            ($L6.scount + ($L6.s0 ? $R6.scount_aux6 : $R6.scount));
        _s1.s1_aux =
            ($L6.s0 || $L6.s0_aux || $L6.s1_aux) ? ($R6.s1 || $R6.s1_aux1) :
            ($L6.s1_aux2 || $R6.s1);
        _s1.scount_aux5 =
            $L6.s1_aux2 ?
            ($L6.scount_aux5 + ($L6.s0 ? $L6.scount : $R6.scount_aux7)) :
            ($L6.scount_aux5 + ($L6.s0 ? $R6.scount_aux6 : $R6.scount));
        _s1.s0_aux = $R6.s0_aux;
        _s1.s1_aux1 =
            ($R6.s1_aux1 && (! $L6.s0_aux)) ? ($L6.s1_aux || $R6.s1) : $R6.s1_aux1;
        _s1.scount_aux6 =
            $L6.s1_aux2 ?
            ($L6.scount_aux6 + ($L6.s1_aux1 ? $R6.scount_aux5 : -1)) :
            ($L6.scount_aux6 + ($L6.s0 ? $R6.scount_aux6 : $R6.scount));
        _s1.s1_aux2 =
            ($L6.s0 || $L6.s1_aux1 || $L6.s1_aux1) ? $R6.s1_aux2 :
            ($R6.s1 || $L6.s1_aux2 || $L6.s1_aux);
        _s1.scount_aux7 =
            $L6.s1_aux2 ?
            ($L6.scount_aux5 + ($L6.s0 ? $L6.scount : $R6.scount_aux7)) :
            ($L6.scount_aux5 + ($L6.s0 ? $R6.scount_aux6 : $R6.scount));
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

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/count10p.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   s0 = false; s1 = false; scount = 0; s1_aux = true; scount_aux5 = 0; s0_aux =
//   true; s1_aux1 = false; scount_aux6 = 0; s1_aux2 = true; scount_aux7 = 0; }
// for(x in a){
//   _s1 = {
//     s0 = x = 1;
//     s1 = (x = 0) && _s1.s0 || _s1.s1;
//     scount = _s1.scount + (if _s1.s1 && (¬ (x = 0)) then 1 else 0);
//     s1_aux = (x = 0) && _s1.s0 || _s1.s1_aux;
//     scount_aux5 =
//     _s1.scount_aux5 + (if _s1.s1_aux && (¬ (x = 0)) then 1 else 0);
//     s0_aux = x = 1;
//     s1_aux1 = (x = 0) && _s1.s0_aux || _s1.s1_aux1;
//     scount_aux6 =
//     _s1.scount_aux6 + (if _s1.s1_aux1 && (¬ (x = 0)) then 1 else 0);
//     s1_aux2 = (x = 0) && _s1.s0_aux || _s1.s1_aux2;
//     scount_aux7 =
//     _s1.scount_aux7 + (if _s1.s1_aux2 && (¬ (x = 0)) then 1 else 0);
//   }
//   }
//   return _s1;
// //--- Splitting predicate: ((a(-1) = 0) && (¬ (a(-2) = 1))) || (a(0) = 0)
// //--- Join function ---
// join($L6,$R6) = {
//   s0 = $R6.s0;
//   s1 = if $L6.s1 || $L6.s0 then $R6.s1_aux1 else $R6.s1;
//   scount =
//   if $L6.s1 then if $L6.s1_aux then $R6.scount_aux7 else -1 else
//     $L6.scount + (if $L6.s0 then $R6.scount_aux6 else $R6.scount);
//   s1_aux =
//   if $L6.s0 || $L6.s0_aux || $L6.s1_aux then $R6.s1 || $R6.s1_aux1 else
//     $L6.s1_aux2 || $R6.s1;
//   scount_aux5 =
//   if $L6.s1_aux2 then
//     $L6.scount_aux5 + (if $L6.s0 then $L6.scount else $R6.scount_aux7) else
//     $L6.scount_aux5 + (if $L6.s0 then $R6.scount_aux6 else $R6.scount);
//   s0_aux = $R6.s0_aux;
//   s1_aux1 =
//   if $R6.s1_aux1 && (¬ $L6.s0_aux) then $L6.s1_aux || $R6.s1 else $R6.s1_aux1;
//   scount_aux6 =
//   if $L6.s1_aux2 then
//     $L6.scount_aux6 + (if $L6.s1_aux1 then $R6.scount_aux5 else -1) else
//     $L6.scount_aux6 + (if $L6.s0 then $R6.scount_aux6 else $R6.scount);
//   s1_aux2 =
//   if $L6.s0 || $L6.s1_aux1 || $L6.s1_aux1 then $R6.s1_aux2 else
//     $R6.s1 || $L6.s1_aux2 || $L6.s1_aux;
//   scount_aux7 =
//   if $L6.s1_aux2 then
//     $L6.scount_aux5 + (if $L6.s0 then $L6.scount else $R6.scount_aux7) else
//     $L6.scount_aux5 + (if $L6.s0 then $R6.scount_aux6 else $R6.scount);
// }
// // Synthesized in (predicate : 0.26 s) + (join : 30.39 s)