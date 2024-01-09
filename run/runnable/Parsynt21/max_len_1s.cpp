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
    int ma = 0; int l = 0; int ma_aux2 = 0; bool ma_aux3 = true; int l_aux = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.ma = std::max(_s1.ma, (!(x == 0) ? _s1.l + 1 : 0));
        _s1.l = (!(x == 0) ? _s1.l + 1 : 0);
        _s1.ma_aux2 = (_s1.ma_aux3 && (!(x == 0)) ? 1 + _s1.ma_aux2 : _s1.ma_aux2);
        _s1.ma_aux3 = _s1.ma_aux3 && (!(x == 0));
        _s1.l_aux = 1 + _s1.l_aux;
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
        _s1.ma = std::max($L4.ma, (!($R4.ma_aux2 == 0) ? $L4.l + $R4.ma : $R4.ma));
        _s1.l =
            $L4.ma_aux3 ? ($R4.ma_aux3 ? $L4.l_aux + $R4.l_aux : $R4.l) :
            ($R4.ma_aux3 ? $L4.l + $R4.l_aux : $R4.l);
        _s1.ma_aux2 = $L4.ma_aux3 ? $L4.ma + $R4.ma_aux2 : $L4.ma_aux2;
        _s1.ma_aux3 = $L4.ma_aux3 && $R4.ma_aux3;
        _s1.l_aux = $L4.l_aux + $R4.l_aux;
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
    printf("%d\n", res.ma);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/max_len_1s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { ma = 0; l = 0; ma_aux2 = 0; ma_aux3 = true; l_aux = 0; }for(x in a){
//                                                                   _s1 = {
//                                                                     ma =
//                                                                     max
//                                                                     (_s1.ma,
//                                                                     (if
//                                                                     ¬ (x = 0)
//                                                                     then
//                                                                     _s1.l + 1
//                                                                     else 0));
//                                                                     l =
//                                                                     if
//                                                                     ¬ (x = 0)
//                                                                     then
//                                                                     _s1.l + 1
//                                                                     else 0;
//                                                                     ma_aux2 =
//                                                                     if
//                                                                     _s1.ma_aux3
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 0))
//                                                                     then
//                                                                     1 +
//                                                                     _s1.ma_aux2
//                                                                     else
//                                                                     _s1.ma_aux2;
//                                                                     ma_aux3 =
//                                                                     _s1.ma_aux3
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 0));
//                                                                     l_aux =
//                                                                     1 +
//                                                                     _s1.l_aux;
//                                                                   }
//                                                                   }
//                                                                   return _s1;
// //--- Splitting predicate: a(0) = 0
// //--- Join function ---
// join($L4,$R4) = {
//   ma = max ($L4.ma, (if ¬ ($R4.ma_aux2 = 0) then $L4.l + $R4.ma else $R4.ma));
//   l =
//   if $L4.ma_aux3 then if $R4.ma_aux3 then $L4.l_aux + $R4.l_aux else $R4.l else
//     if $R4.ma_aux3 then $L4.l + $R4.l_aux else $R4.l;
//   ma_aux2 = if $L4.ma_aux3 then $L4.ma + $R4.ma_aux2 else $L4.ma_aux2;
//   ma_aux3 = $L4.ma_aux3 && $R4.ma_aux3;
//   l_aux = $L4.l_aux + $R4.l_aux;
// }
// // Synthesized in (predicate : 0.27 s) + (join : 17.14 s)