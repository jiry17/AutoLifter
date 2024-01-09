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
    int ml = 0; int cl = 0; int ml_aux2 = 0; bool ml_aux3 = true;
    int cl_aux = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.ml = std::max(_s1.ml, (x == 1 ? _s1.cl + 1 : 0));
        _s1.cl = (x == 1) ? _s1.cl + 1 : 0;
        _s1.ml_aux2 = (_s1.ml_aux3 && (x == 1) ? 1 + _s1.ml_aux2 : _s1.ml_aux2);
        _s1.ml_aux3 = _s1.ml_aux3 && (x == 1);
        _s1.cl_aux = 1 + _s1.cl_aux;
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
        _s1.ml =
            std::max($L4.ml, ($R4.ml_aux2 == $R4.ml ? $R4.ml_aux2 + $L4.cl : $R4.ml));
        _s1.cl = $R4.ml_aux3 ? $R4.cl_aux + $L4.cl : $R4.cl;
        _s1.ml_aux2 =
            $L4.ml_aux3 ? $L4.ml + $R4.ml_aux2 :
            $L4.ml_aux3 ? $L4.ml + $L4.ml : $L4.ml_aux2;
        _s1.ml_aux3 = $L4.ml_aux3 && $R4.ml_aux3;
        _s1.cl_aux = $L4.cl_aux + $R4.cl_aux;
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
    printf("%d\n", res.ml);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/longest1s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { ml = 0; cl = 0; ml_aux2 = 0; ml_aux3 = true; cl_aux = 0; }for(x in a){
//                                                                     _s1 = {
//                                                                     ml =
//                                                                     max
//                                                                     (_s1.ml,
//                                                                     (if 
//                                                                     x = 1 then
//                                                                     _s1.cl + 1
//                                                                     else 0));
//                                                                     cl =
//                                                                     if 
//                                                                     x = 1 then
//                                                                     _s1.cl + 1
//                                                                     else 0;
//                                                                     ml_aux2 =
//                                                                     if
//                                                                     _s1.ml_aux3
//                                                                     && 
//                                                                     (x = 1)
//                                                                     then
//                                                                     1 +
//                                                                     _s1.ml_aux2
//                                                                     else
//                                                                     _s1.ml_aux2;
//                                                                     ml_aux3 =
//                                                                     _s1.ml_aux3
//                                                                     && 
//                                                                     (x = 1);
//                                                                     cl_aux =
//                                                                     1 +
//                                                                     _s1.cl_aux;
//                                                                     }
//                                                                     }
//                                                                     return _s1;
// //--- Splitting predicate: ¬ (a(0) = 1)
// //--- Join function ---
// join($L4,$R4) = {
//   ml =
//   max ($L4.ml, (if $R4.ml_aux2 = $R4.ml then $R4.ml_aux2 + $L4.cl else $R4.ml));
//   cl = if $R4.ml_aux3 then $R4.cl_aux + $L4.cl else $R4.cl;
//   ml_aux2 =
//   if $L4.ml_aux3 then $L4.ml + $R4.ml_aux2 else
//     if $L4.ml_aux3 then $L4.ml + $L4.ml else $L4.ml_aux2;
//   ml_aux3 = $L4.ml_aux3 && $R4.ml_aux3;
//   cl_aux = $L4.cl_aux + $R4.cl_aux;
// }
// // Synthesized in (predicate : 0.15 s) + (join : 11.88 s)