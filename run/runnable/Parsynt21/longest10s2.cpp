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
    bool s1 = false; int ml = 0; int cl = 0; bool ml_aux5 = true;
    int ml_aux6 = 0; int cl_aux = 0; bool s1_aux = true; int ml_aux7 = 0;
    int cl_aux3 = 0; bool ml_aux5_aux1 = true; int ml_aux6_aux1 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.s1 = (x == 0) && _s1.s1 || (x == 1);
        _s1.ml = std::max((_s1.s1 || (x == 2) ? _s1.cl + 1 : 0), _s1.ml);
        _s1.cl = _s1.s1 || (x == 2) ? _s1.cl + 1 : 0;
        _s1.ml_aux5 = _s1.ml_aux5 && _s1.s1 || (x == 2);
        _s1.ml_aux6 =
            _s1.ml_aux5 && _s1.s1 || (x == 2) ? _s1.ml_aux6 + 1 : _s1.ml_aux6;
        _s1.cl_aux = 1 + _s1.cl_aux;
        _s1.s1_aux = (x == 0) && _s1.s1_aux || (x == 1);
        _s1.ml_aux7 =
            std::max((_s1.s1_aux || (x == 2) ? _s1.cl_aux3 + 1 : 0), _s1.ml_aux7);
        _s1.cl_aux3 = _s1.s1_aux || (x == 2) ? _s1.cl_aux3 + 1 : 0;
        _s1.ml_aux5_aux1 = _s1.ml_aux5_aux1 && _s1.s1_aux || (x == 2);
        _s1.ml_aux6_aux1 =
            _s1.ml_aux5_aux1 && _s1.s1_aux || (x == 2) ? _s1.ml_aux6_aux1 + 1 : _s1.ml_aux6_aux1;
        return _s1;
    } else {
        result $L7, $R7;
        int mid = l + r >> 1;
#pragma omp task shared($L7) if(r - l > lim)
        $L7 = dac(l, mid);
#pragma omp task shared($R7) if(r - l > lim)
        $R7 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.s1 = $R7.s1_aux ? $R7.s1 : $L7.s1;
        _s1.ml = std::max
            (($L7.ml_aux5_aux1 || ($L7.ml == $L7.cl_aux3) ? $L7.ml + $R7.ml_aux6
            : $L7.ml),
            $R7.ml);
        _s1.cl = $R7.ml_aux5 ? $L7.cl + $R7.cl_aux : $R7.cl;
        _s1.ml_aux5 =
            $L7.s1_aux ? $L7.ml_aux5 : $R7.ml_aux5 && $R7.s1 || $L7.ml_aux5;
        _s1.ml_aux6 =
            $L7.ml_aux5 ?
            $L7.ml_aux5 ? $L7.ml_aux6_aux1 + $R7.ml_aux6 : $L7.ml :
            $L7.ml_aux6;
        _s1.cl_aux = $L7.cl_aux + $R7.cl_aux;
        _s1.s1_aux = $L7.s1_aux && $R7.s1_aux;
        _s1.ml_aux7 = std::max
            (($L7.s1_aux || ($R7.ml_aux6_aux1 == $R7.ml_aux6) ?
            $L7.cl_aux3 + $R7.ml_aux7 : $R7.ml),
            $L7.ml_aux7);
        _s1.cl_aux3 =
            $R7.ml_aux5_aux1 ?
            $L7.s1_aux || ($R7.ml_aux6_aux1 == $R7.cl) ? $R7.ml_aux7 + $L7.cl_aux3
            : $R7.cl
            : $R7.cl_aux3;
        _s1.ml_aux5_aux1 =
            $R7.ml_aux5_aux1 && ($L7.ml_aux5_aux1 && $R7.ml_aux5 || $L7.s1_aux);
        _s1.ml_aux6_aux1 =
            $L7.s1_aux ?
            $L7.s1_aux ? $L7.ml_aux7 + $R7.ml_aux7 : $R7.ml_aux7 :
            $L7.ml_aux5_aux1 ? $R7.ml_aux6 + $L7.cl_aux : $L7.ml_aux6_aux1;
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

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/longest10s2.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   s1 = false; ml = 0; cl = 0; ml_aux5 = true; ml_aux6 = 0; cl_aux = 0; s1_aux =
//   true; ml_aux7 = 0; cl_aux3 = 0; ml_aux5_aux1 = true; ml_aux6_aux1 = 0; }
// for(x in a){
//   _s1 = {
//     s1 = (x = 0) && _s1.s1 || (x = 1);
//     ml = std::max((if _s1.s1 || (x = 2) then _s1.cl + 1 else 0), _s1.ml);
//     cl = if _s1.s1 || (x = 2) then _s1.cl + 1 else 0;
//     ml_aux5 = _s1.ml_aux5 && _s1.s1 || (x = 2);
//     ml_aux6 =
//     if _s1.ml_aux5 && _s1.s1 || (x = 2) then _s1.ml_aux6 + 1 else _s1.ml_aux6;
//     cl_aux = 1 + _s1.cl_aux;
//     s1_aux = (x = 0) && _s1.s1_aux || (x = 1);
//     ml_aux7 =
//     std::max((if _s1.s1_aux || (x = 2) then _s1.cl_aux3 + 1 else 0), _s1.ml_aux7);
//     cl_aux3 = if _s1.s1_aux || (x = 2) then _s1.cl_aux3 + 1 else 0;
//     ml_aux5_aux1 = _s1.ml_aux5_aux1 && _s1.s1_aux || (x = 2);
//     ml_aux6_aux1 =
//     if _s1.ml_aux5_aux1 && _s1.s1_aux || (x = 2) then _s1.ml_aux6_aux1 + 1 else
//       _s1.ml_aux6_aux1;
//   }
//   }
//   return _s1;
// //--- Splitting predicate: ¬ (¬ ((a(-1) = 0) && (a(-1) = 1))) || (a(0) = 2)
// //--- Join function ---
// join($L7,$R7) = {
//   s1 = if $R7.s1_aux then $R7.s1 else $L7.s1;
//   ml =
//   max
//     ((if $L7.ml_aux5_aux1 || ($L7.ml = $L7.cl_aux3) then $L7.ml + $R7.ml_aux6
//       else $L7.ml),
//     $R7.ml);
//   cl = if $R7.ml_aux5 then $L7.cl + $R7.cl_aux else $R7.cl;
//   ml_aux5 =
//   if $L7.s1_aux then $L7.ml_aux5 else $R7.ml_aux5 && $R7.s1 || $L7.ml_aux5;
//   ml_aux6 =
//   if $L7.ml_aux5 then
//     if $L7.ml_aux5 then $L7.ml_aux6_aux1 + $R7.ml_aux6 else $L7.ml else
//     $L7.ml_aux6;
//   cl_aux = $L7.cl_aux + $R7.cl_aux;
//   s1_aux = $L7.s1_aux && $R7.s1_aux;
//   ml_aux7 =
//   max
//     ((if $L7.s1_aux || ($R7.ml_aux6_aux1 = $R7.ml_aux6) then
//       $L7.cl_aux3 + $R7.ml_aux7 else $R7.ml),
//     $L7.ml_aux7);
//   cl_aux3 =
//   if $R7.ml_aux5_aux1 then
//     if $L7.s1_aux || ($R7.ml_aux6_aux1 = $R7.cl) then $R7.ml_aux7 + $L7.cl_aux3
//       else $R7.cl
//     else $R7.cl_aux3;
//   ml_aux5_aux1 =
//   $R7.ml_aux5_aux1 && ($L7.ml_aux5_aux1 && $R7.ml_aux5 || $L7.s1_aux);
//   ml_aux6_aux1 =
//   if $L7.s1_aux then
//     if $L7.s1_aux then $L7.ml_aux7 + $R7.ml_aux7 else $R7.ml_aux7 else
//     if $L7.ml_aux5_aux1 then $R7.ml_aux6 + $L7.cl_aux else $L7.ml_aux6_aux1;
// }
// // Synthesized in (predicate : 0.78 s) + (join : 43.42 s)