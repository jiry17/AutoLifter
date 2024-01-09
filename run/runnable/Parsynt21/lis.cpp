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
    int cl = 0;
    int ml = 0;
    int prev = 0;
    int cl_aux = 0;
    int ml_aux2 = w[1];
    bool ml_aux3 = true;
    int ml_aux4 = w[0];
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.cl = _s1.prev < x ? _s1.cl + 1 : 0;
        _s1.ml = std::max((_s1.prev < x ? _s1.cl + 1 : 0), _s1.ml);
        _s1.prev = x;
        _s1.cl_aux = 1 + _s1.cl_aux;
        _s1.ml_aux2 = (_s1.ml_aux3 && (_s1.prev < x)) ? 1 + _s1.ml_aux2 : _s1.ml_aux2;
        _s1.ml_aux3 = _s1.ml_aux3 && (_s1.prev < x);
        _s1.ml_aux4 = _s1.ml_aux4;
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
        _s1.cl =
            ($R1.cl == $R1.cl_aux) ?
            ($L1.prev < $R1.ml_aux4 ?
            (((((($R1.cl + (- $R1.prev)) + (- $R1.cl)) + $L1.cl) + $R1.prev) +
                $R1.cl_aux)
            + 1) + -1
            : $R1.cl_aux + (- 1))
            : $R1.cl;
        _s1.ml =
            std::max(($L1.prev < $R1.ml_aux4 ? $L1.cl + $R1.ml : $R1.ml_aux2),
            $L1.ml);
        _s1.prev = $R1.prev;
        _s1.cl_aux = $L1.cl_aux + $R1.cl_aux;
        _s1.ml_aux2 =
            ($L1.ml_aux3 && ($L1.prev < $R1.ml_aux4)) ? $R1.ml + $L1.ml_aux2 :
            $L1.ml_aux2;
        _s1.ml_aux3 = $R1.ml_aux3 && (($L1.prev < $R1.ml_aux4) && $L1.ml_aux3);
        _s1.ml_aux4 = $L1.ml_aux4;
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
    printf("%d\n", res.cl);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/lis.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   cl = 0; ml = 0; prev = 0; cl_aux = 0; ml_aux2 = a[1]; ml_aux3 = true;
//   ml_aux4 = a(0); }for(x in a){
//                      _s1 = {
//                        cl = if _s1.prev < x then _s1.cl + 1 else 0;
//                        ml =
//                        max ((if _s1.prev < x then _s1.cl + 1 else 0), _s1.ml);
//                        prev = x;
//                        cl_aux = 1 + _s1.cl_aux;
//                        ml_aux2 =
//                        if _s1.ml_aux3 && (_s1.prev < x) then 1 + _s1.ml_aux2
//                          else _s1.ml_aux2;
//                        ml_aux3 = _s1.ml_aux3 && (_s1.prev < x);
//                        ml_aux4 = _s1.ml_aux4;
//                      }
//                      }
//                      return _s1;
// //--- Splitting predicate: a(-1) >= a(0)
// //--- Join function ---
// join($L1,$R1) = {
//   cl =
//   if $R1.cl = $R1.cl_aux then
//     if $L1.prev < $R1.ml_aux4 then
//       (((((($R1.cl + (- $R1.prev)) + (- $R1.cl)) + $L1.cl) + $R1.prev) +
//         $R1.cl_aux)
//        + 1) + -1
//       else $R1.cl_aux + (- 1)
//     else $R1.cl;
//   ml =
//   max ((if $L1.prev < $R1.ml_aux4 then $L1.cl + $R1.ml else $R1.ml_aux2),
//     $L1.ml);
//   prev = $R1.prev;
//   cl_aux = $L1.cl_aux + $R1.cl_aux;
//   ml_aux2 =
//   if $L1.ml_aux3 && ($L1.prev < $R1.ml_aux4) then $R1.ml + $L1.ml_aux2 else
//     $L1.ml_aux2;
//   ml_aux3 = $R1.ml_aux3 && (($L1.prev < $R1.ml_aux4) && $L1.ml_aux3);
//   ml_aux4 = $L1.ml_aux4;
// }
// // Synthesized in (predicate : 0.18 s) + (join : 17.18 s)