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
    int cl = 0; int ml_tmp = 0; int ml = 0; int cl_aux = 0; int ml_tmp_aux2 = 0;
    bool ml_tmp_aux3 = true;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.cl = !x ? _s1.cl + 1 : 0;
        _s1.ml_tmp = std::max(_s1.ml_tmp, (!x ? _s1.cl + 1 : 0));
        _s1.ml =
            ((std::max(_s1.ml_tmp, (!x ? _s1.cl + 1 : 0))) % 2) == 0 ?
            std::max(_s1.ml_tmp, (!x ? _s1.cl + 1 : 0)) : _s1.ml;
        _s1.cl_aux = 1 + _s1.cl_aux;
        _s1.ml_tmp_aux2 =
            _s1.ml_tmp_aux3 && (!x) ? 1 + _s1.ml_tmp_aux2 :
            _s1.ml_tmp_aux2;
        _s1.ml_tmp_aux3 = _s1.ml_tmp_aux3 && (!x);
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
        _s1.cl = $R4.ml_tmp_aux3 ? $L4.cl + $R4.cl_aux : $R4.cl;
        _s1.ml_tmp = std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)));
        _s1.ml =
            ((std::max((std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)))),
            1))
            % 2) == 1
            ? std::max($R4.ml, (std::max(($R4.ml + $L4.ml), 0))) :
            std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)));
        _s1.cl_aux = $L4.cl_aux + $R4.cl_aux;
        _s1.ml_tmp_aux2 =
            $L4.ml_tmp_aux3 ? $R4.ml_tmp_aux2 + $L4.ml_tmp : $L4.ml_tmp_aux2;
        _s1.ml_tmp_aux3 = $L4.ml_tmp_aux3 && $R4.ml_tmp_aux3;
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

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/longest00s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   cl = 0; ml_tmp = 0; ml = 0; cl_aux = 0; ml_tmp_aux2 = 0; ml_tmp_aux3 = true;
//   }for(x in a){
//      _s1 = {
//        cl = if ¬ x then _s1.cl + 1 else 0;
//        ml_tmp = std::max(_s1.ml_tmp, (if ¬ x then _s1.cl + 1 else 0));
//        ml =
//        if ((std::max(_s1.ml_tmp, (if ¬ x then _s1.cl + 1 else 0))) % 2) = 0 then
//          std::max(_s1.ml_tmp, (if ¬ x then _s1.cl + 1 else 0)) else _s1.ml;
//        cl_aux = 1 + _s1.cl_aux;
//        ml_tmp_aux2 =
//        if _s1.ml_tmp_aux3 && (¬ x) then 1 + _s1.ml_tmp_aux2 else
//          _s1.ml_tmp_aux2;
//        ml_tmp_aux3 = _s1.ml_tmp_aux3 && (¬ x);
//      }
//      }
//      return _s1;
// //--- Splitting predicate: a(0)
// //--- Join function ---
// join($L4,$R4) = {
//   cl = if $R4.ml_tmp_aux3 then $L4.cl + $R4.cl_aux else $R4.cl;
//   ml_tmp = std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)));
//   ml =
//   if
//     ((std::max((std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)))),
//       1))
//      % 2) = 1
//     then std::max($R4.ml, (std::max(($R4.ml + $L4.ml), 0))) else
//     std::max($L4.ml_tmp, (std::max(($L4.cl + $R4.ml_tmp_aux2), $R4.ml_tmp)));
//   cl_aux = $L4.cl_aux + $R4.cl_aux;
//   ml_tmp_aux2 =
//   if $L4.ml_tmp_aux3 then $R4.ml_tmp_aux2 + $L4.ml_tmp else $L4.ml_tmp_aux2;
//   ml_tmp_aux3 = $L4.ml_tmp_aux3 && $R4.ml_tmp_aux3;
// }
// // Synthesized in (predicate : 0.22 s) + (join : 100.37 s)