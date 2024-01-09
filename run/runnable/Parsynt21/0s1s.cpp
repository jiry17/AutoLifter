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
    bool an = true;
    bool bn = true;
    bool an_aux = false;
    bool bn_aux1 = true;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.an = x && _s1.an;
        _s1.bn = (!x) || _s1.an && _s1.bn;
        _s1.an_aux = x && _s1.an_aux;
        _s1.bn_aux1 = (!x) || _s1.an_aux && _s1.bn_aux1;
        return _s1;
    } else {
        result $L5, $R5;
        int mid = l + r >> 1;
#pragma omp task shared($L5) if(r - l > lim)
        $L5 = dac(l, mid);
#pragma omp task shared($R5) if(r - l > lim)
        $R5 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.an = (! $R5.an_aux) && ($R5.an && $L5.an);
        _s1.bn = $R5.bn_aux1 ? $R5.bn && $L5.bn : $R5.bn && $L5.an;
        _s1.an_aux = false;
        _s1.bn_aux1 = $R5.bn ? (! $R5.bn) || $L5.bn_aux1 && $R5.bn_aux1 : $R5.an_aux;
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
    printf("%d\n", res.an);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/0s1s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = { an = true; bn = true; an_aux = false; bn_aux1 = true; }for(x in a){
//                                                                  _s1 = {
//                                                                    an =
//                                                                    x && _s1.an;
//                                                                    bn =
//                                                                    (¬ x) ||
//                                                                     _s1.an &&
//                                                                     _s1.bn;
//                                                                    an_aux =
//                                                                    x &&
//                                                                     _s1.an_aux;
//                                                                    bn_aux1 =
//                                                                    (¬ x) ||
//                                                                     _s1.an_aux
//                                                                     &&
//                                                                     _s1.bn_aux1;
//                                                                  }
//                                                                  }
//                                                                  return _s1;
// //--- Splitting predicate: (empty)
// //--- Join function ---
// join($L5,$R5) = {
//   an = (¬ $R5.an_aux) && ($R5.an && $L5.an);
//   bn = if $R5.bn_aux1 then $R5.bn && $L5.bn else $R5.bn && $L5.an;
//   an_aux = false;
//   bn_aux1 =
//   if $R5.bn then (¬ $R5.bn) || $L5.bn_aux1 && $R5.bn_aux1 else $R5.an_aux;
// }
// // Synthesized in (predicate : 0.03 s) + (join : 5.77 s)