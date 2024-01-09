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
    bool s1 = false;
    bool s2 = false;
    int c = 0;
    bool s2_aux = true;
    int c_aux3 = 0;
    bool s1_aux = true;
    bool s2_aux1 = false;
    int c_aux4 = 0;
    bool s2_aux2 = true;
    int c_aux5 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.s1 = (x == 1);
        _s1.s2 = (x == 2) && _s1.s1 || _s1.s2;
        _s1.c = _s1.c + ((x == 3) && _s1.s2 || _s1.s1 ? 1 : 0);
        _s1.s2_aux = (x == 2) && _s1.s1 || _s1.s2_aux;
        _s1.c_aux3 = _s1.c_aux3 + ((x == 3) && _s1.s2_aux || _s1.s1 ? 1 : 0);
        _s1.s1_aux = (x == 1);
        _s1.s2_aux1 = (x == 2) && _s1.s1_aux || _s1.s2_aux1;
        _s1.c_aux4 = _s1.c_aux4 + ((x == 3) && _s1.s2_aux1 || _s1.s1_aux ? 1 : 0);
        _s1.s2_aux2 = ((x == 2) && _s1.s2_aux2 || _s1.s1_aux ? 1 : 0);
        _s1.c_aux5 = _s1.c_aux5 + ((x == 3) && _s1.s2_aux2 || _s1.s1_aux ? 1 : 0);
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
        _s1.s1 = $R6.s1;
        _s1.s2 = $L6.s2 || $L6.s1 ? $R6.s2_aux : $R6.s2;
        _s1.c = $L6.s2_aux ? ($L6.s2 ? $R6.c_aux4 : $R6.c) :
            ($L6.c + ($L6.s1_aux ? $R6.c_aux3 : $R6.c));
        _s1.s2_aux = ($L6.s1 || $L6.s1_aux || $L6.s1_aux) ?
            ($R6.s2_aux || $L6.s2_aux || $L6.s2_aux1) :
            ($L6.s1_aux || $L6.s2_aux1 ? $L6.s1 || $R6.s2_aux1 :
            $L6.s1_aux || $R6.s2 || $L6.s2_aux1);
        _s1.c_aux3 = $L6.s2_aux ? ($R6.c_aux4 + ($L6.s1 ? -1 : $L6.c_aux5)) :
            ($L6.c_aux3 + ($L6.s1 ? $R6.c_aux3 : $R6.c));
        _s1.s1_aux = $R6.s1_aux;
        _s1.s2_aux1 = ($R6.s2_aux1 && (! $L6.s1_aux)) ? ($L6.s2_aux || $R6.s2) : $R6.s2_aux1;
        _s1.c_aux4 = $L6.s2_aux ? ($R6.c_aux3 + $L6.c_aux3) :
            ($L6.c_aux3 + ($L6.s1_aux ? $R6.c_aux4 : $R6.c));
        _s1.s2_aux2 = ($L6.s2_aux1 || $L6.s1_aux || $L6.s2) ? ($R6.s2_aux1 || $R6.s2_aux) :
            ($R6.s2 || $L6.s1_aux || $L6.s1_aux);
        _s1.c_aux5 = $L6.s2_aux2 ? ($R6.c_aux4 + $L6.c_aux5):
            ($L6.c_aux4 + ($L6.s1 ? $R6.c_aux5 : $R6.c));
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
    printf("%d\n", res.s1);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/count1s2s3s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   s1 = false; s2 = false; c = 0; s2_aux = true; c_aux3 = 0; s1_aux = true;
//   s2_aux1 = false; c_aux4 = 0; s2_aux2 = true; c_aux5 = 0; }for(x in a){
//                                                               _s1 = {
//                                                                 s1 = x = 1;
//                                                                 s2 =
//                                                                 (x = 2) &&
//                                                                   _s1.s1 ||
//                                                                     _s1.s2;
//                                                                 c =
//                                                                 _s1.c +
//                                                                   (if
//                                                                    (x = 3) &&
//                                                                     _s1.s2 ||
//                                                                     _s1.s1
//                                                                    then 1 else
//                                                                    0);
//                                                                 s2_aux =
//                                                                 (x = 2) &&
//                                                                   _s1.s1 ||
//                                                                     _s1.s2_aux;
//                                                                 c_aux3 =
//                                                                 _s1.c_aux3 +
//                                                                   (if
//                                                                    (x = 3) &&
//                                                                     _s1.s2_aux
//                                                                     || _s1.s1
//                                                                    then 1 else
//                                                                    0);
//                                                                 s1_aux = x = 1;
//                                                                 s2_aux1 =
//                                                                 (x = 2) &&
//                                                                   _s1.s1_aux ||
//                                                                     _s1.s2_aux1;
//                                                                 c_aux4 =
//                                                                 _s1.c_aux4 +
//                                                                   (if
//                                                                    (x = 3) &&
//                                                                     _s1.s2_aux1
//                                                                     ||
//                                                                     _s1.s1_aux
//                                                                    then 1 else
//                                                                    0);
//                                                                 s2_aux2 =
//                                                                 (x = 2) &&
//                                                                   _s1.s1_aux ||
//                                                                     _s1.s2_aux2;
//                                                                 c_aux5 =
//                                                                 _s1.c_aux5 +
//                                                                   (if
//                                                                    (x = 3) &&
//                                                                     _s1.s2_aux2
//                                                                     ||
//                                                                     _s1.s1_aux
//                                                                    then 1 else
//                                                                    0);
//                                                               }
//                                                               }
//                                                               return _s1;
// //--- Splitting predicate:
// ¬ ((a(0) = 3) && (¬ ((a(-1) = 2) && (¬ (a(-2) = 1)))) || (¬ (a(-1) = 1)))
// //--- Join function ---
// join($L6,$R6) = {
//   s1 = $R6.s1;
//   s2 = if $L6.s2 || $L6.s1 then $R6.s2_aux else $R6.s2;
//   c =
//   if $L6.s2_aux then if $L6.s2 then $R6.c_aux4 else $R6.c else
//     $L6.c + (if $L6.s1_aux then $R6.c_aux3 else $R6.c);
//   s2_aux =
//   if $L6.s1 || $L6.s1_aux || $L6.s1_aux then
//     $R6.s2_aux || $L6.s2_aux || $L6.s2_aux1 else
//     if $L6.s1_aux || $L6.s2_aux1 then $L6.s1 || $R6.s2_aux1 else
//       $L6.s1_aux || $R6.s2 || $L6.s2_aux1;
//   c_aux3 =
//   if $L6.s2_aux then $R6.c_aux4 + (if $L6.s1 then -1 else $L6.c_aux5) else
//     $L6.c_aux3 + (if $L6.s1 then $R6.c_aux3 else $R6.c);
//   s1_aux = $R6.s1_aux;
//   s2_aux1 =
//   if $R6.s2_aux1 && (¬ $L6.s1_aux) then $L6.s2_aux || $R6.s2 else $R6.s2_aux1;
//   c_aux4 =
//   if $L6.s2_aux then $R6.c_aux3 + $L6.c_aux3 else
//     $L6.c_aux3 + (if $L6.s1_aux then $R6.c_aux4 else $R6.c);
//   s2_aux2 =
//   if $L6.s2_aux1 || $L6.s1_aux || $L6.s2 then $R6.s2_aux1 || $R6.s2_aux else
//     $R6.s2 || $L6.s1_aux || $L6.s1_aux;
//   c_aux5 =
//   if $L6.s2_aux2 then $R6.c_aux4 + $L6.c_aux5 else
//     $L6.c_aux4 + (if $L6.s1 then $R6.c_aux5 else $R6.c);
// }
// // Synthesized in (predicate : 0.22 s) + (join : 24.58 s)