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
    bool s1 = false; bool s2 = false; int cl = 0; int ml = 0; int ml_odd = 0;
    bool s2_aux = true; bool s1_aux = false; int cl_aux2 = 0; int ml_aux6 = 0;
    int ml_odd_aux = 0; bool s1_aux2 = true; int cl_aux4 = 0; int ml_aux8 = 0;
    int ml_odd_aux2 = 0; bool ml_aux4_aux3 = true; int ml_aux5_aux3 = 0;
    bool s1_aux4 = true; int cl_aux6 = 0; int ml_aux10 = 0; int ml_odd_aux4 = 0;
    bool ml_aux4_aux5 = true; int ml_aux5_aux5 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.s1 = _s1.s2 && (x == 1);
        _s1.s2 = (x == 0);
        _s1.cl = _s1.s1 ? _s1.cl + 1 : 0;
        _s1.ml = std::max(_s1.ml, (_s1.s1 ? _s1.cl + 1 : 0));
        _s1.ml_odd = (((std::max(_s1.ml,
            (_s1.s1 ? _s1.cl + 1 : 0)))
            % 2) == 1)
            ?
            std::max(_s1.ml,
            (_s1.s1 ? _s1.cl + 1 : 0))
            : _s1.ml_odd;
        _s1.s2_aux = x = 0;
        _s1.s1_aux = _s1.s2_aux && (x == 1);
        _s1.cl_aux2 =
            _s1.s1_aux ? _s1.cl_aux2 + 1 : 0;
        _s1.ml_aux6 =
            std::max(_s1.ml_aux6,
            (_s1.s1_aux ? _s1.cl_aux2 + 1 : 0));
        _s1.ml_odd_aux = (((std::max(_s1.ml_aux6,
            (_s1.s1_aux ? _s1.cl_aux2 + 1 : 0)))
            % 2) == 1)
            ?
            std::max(_s1.ml_aux6,
            (_s1.s1_aux ? _s1.cl_aux2 + 1 : 0))
            : _s1.ml_odd_aux;
        _s1.s1_aux2 = _s1.s2 && (x == 1);
        _s1.cl_aux4 =
            _s1.s1_aux2 ? _s1.cl_aux4 + 1 : 0;
        _s1.ml_aux8 =
            std::max(_s1.ml_aux8,
            (_s1.s1_aux2 ? _s1.cl_aux4 + 1 : 0));
        _s1.ml_odd_aux2 = (((std::max(_s1.ml_aux8,
            (_s1.s1_aux2 ? _s1.cl_aux4 + 1 : 0)))
            % 2) == 1)
            ?
            std::max(_s1.ml_aux8,
            (_s1.s1_aux2 ? _s1.cl_aux4 + 1 : 0))
            : _s1.ml_odd_aux2;
        _s1.ml_aux4_aux3 = _s1.ml_aux4_aux3 && _s1.s1_aux2;
        _s1.ml_aux5_aux3 =
            _s1.ml_aux4_aux3 && _s1.s1_aux2 ?
            _s1.ml_aux5_aux3 + 1 : _s1.ml_aux5_aux3;
        _s1.s1_aux4 = _s1.s2_aux && (x = 1);
        _s1.cl_aux6 =
            _s1.s1_aux4 ? _s1.cl_aux6 + 1 : 0;
        _s1.ml_aux10 =
            std::max(_s1.ml_aux10,
            (_s1.s1_aux4 ? _s1.cl_aux6 + 1 : 0));
        _s1.ml_odd_aux4 = (((std::max(_s1.ml_aux10,
            (_s1.s1_aux4 ? _s1.cl_aux6 + 1 : 0)))
            % 2) == 1)
            ?
            std::max(_s1.ml_aux10,
            (_s1.s1_aux4 ? _s1.cl_aux6 + 1 : 0))
            : _s1.ml_odd_aux4;
        _s1.ml_aux4_aux5 = _s1.ml_aux4_aux5 && _s1.s1_aux4;
        _s1.ml_aux5_aux5 =
            _s1.ml_aux4_aux5 && _s1.s1_aux4 ?
            _s1.ml_aux5_aux5 + 1 : _s1.ml_aux5_aux5;
        return _s1;
    } else {
        result $L11, $R11;
        int mid = l + r >> 1;
#pragma omp task shared($L11) if(r - l > lim)
        $L11 = dac(l, mid);
#pragma omp task shared($R11) if(r - l > lim)
        $R11 = dac(mid + 1, r);
#pragma omp taskwait
        result _s1;
        _s1.s1 =
            $L11.s1 ? $R11.s1_aux && $R11.s1_aux2 : $R11.s1_aux4 && $R11.s1_aux2;
        _s1.s2 = $R11.s2;
        _s1.cl =
            $L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
            $L11.s2_aux ? -1 : $R11.ml;
        _s1.ml =
            $L11.s2 ?
            std::max ($R11.ml_odd_aux,
            ($L11.s1_aux4 ?
            ($L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
                $L11.s2_aux ? -1 : $R11.ml) + $L11.ml_aux5_aux5
            : $L11.cl_aux4))
            :
            std::max
            (($L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
                $L11.s2_aux ? -1 : $R11.ml),
            ($L11.s1 ? $R11.ml_aux8 : $L11.ml));
        _s1.ml_odd =
            $L11.s2 ?
            std::max ($R11.ml_odd_aux,
            ($L11.s1_aux4 ?
            ($L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
                $L11.s2_aux ? -1 : $R11.ml) + $L11.ml_aux5_aux5
            : $L11.cl_aux4))
            :
            std::max
            (($L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
                $L11.s2_aux ? -1 : $R11.ml),
            ($L11.s1 ? $R11.ml_aux8 : $L11.ml));
        _s1.s2_aux = $R11.s2_aux;
        _s1.s1_aux = (!$R11.s2) && ($R11.s1_aux4 && $R11.s1_aux);
        _s1.cl_aux2 = $L11.s2 ? $R11.cl_aux2 : $R11.cl_aux4;
        _s1.ml_aux6 =
            $L11.s2_aux ?
            std::max ($R11.ml_odd_aux,
            ($L11.s2_aux ? $L11.ml_odd_aux : $R11.ml_aux10))
            :
            std::max ($L11.ml_odd_aux,
            ($L11.s1_aux4 ? $L11.cl + $R11.ml_aux8 : $R11.ml_odd));
        _s1.ml_odd_aux =
            $L11.s2_aux ?
            std::max ($R11.ml_odd_aux,
            ($L11.s2_aux ? $L11.ml_odd_aux : $R11.ml_aux10))
            :
            std::max ($L11.ml_odd_aux,
            ($L11.s1_aux4 ? $L11.cl + $R11.ml_aux8 : $R11.ml_odd));
        _s1.s1_aux2 = $R11.s1 && $R11.s1_aux;
        _s1.cl_aux4 =
            $L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
            $L11.s2_aux ? -1 : $R11.ml;
        _s1.ml_aux8 =
            std::max (1,
            ($R11.s1_aux2 ?
            ($L11.s2 ? $R11.s1_aux2 ? 0 : $R11.cl_aux2 :
            $L11.s2_aux ? -1 : $R11.ml) + -1
            : 0));
        _s1.ml_odd_aux2 =
            ($R11.cl_aux4 == $R11.ml_aux6) ?
            (($L11.cl_aux4 == 1) ?
            ((($R11.ml_odd_aux2 + $L11.cl_aux2) + $L11.cl_aux4) + (- $L11.ml_aux8)) +
                (- $L11.ml_odd_aux2)
            : ((- $R11.cl_aux2) + $R11.cl_aux4) + $R11.ml_odd_aux2)
            : $R11.ml_odd_aux2;
        _s1.ml_aux4_aux3 =
            $R11.s1_aux4 ? $L11.ml_aux4_aux3 && ($R11.ml_aux4_aux3 && $R11.s2) :
            $L11.ml_aux4_aux3 && $L11.ml_aux4_aux5;
        _s1.ml_aux5_aux3 = $R11.ml_aux5_aux3;
        _s1.s1_aux4 = $R11.s2 ? ($R11.s1_aux4 && $L11.s2_aux) : $R11.s1_aux4;
        _s1.cl_aux6 =
            $R11.ml_aux4_aux5 ? ($L11.s2_aux ? $R11.ml_aux10 + -1 : $R11.cl)
            : $R11.cl_aux6;
        _s1.ml_aux10 =
            $R11.ml_aux4_aux3 ?
            std::max ($L11.cl_aux6,
            ($R11.s1 ?
            ($R11.ml_aux4_aux5 ?
                ($L11.s2_aux ? $R11.ml_aux10 + -1 : $R11.cl) : $R11.cl_aux6)
                + $L11.cl
            : -1))
            :
            std::max ($L11.ml_aux5_aux5,
            ($L11.ml_aux4_aux3 ?
            ($R11.ml_aux4_aux5 ?
                ($L11.s2_aux ? $R11.ml_aux10 + -1 : $R11.cl) : $R11.cl_aux6)
                + $L11.cl_aux2
            : $R11.ml_odd_aux4));
        _s1.ml_odd_aux4 =
            $R11.ml_aux10 = $R11.cl_aux4 ?
            $L11.ml_aux10 = 1 ?
            ((((((((((((($R11.cl_aux2 + (- $R11.ml_aux6)) + (- $R11.ml_odd_aux)) +
                        (- $R11.cl_aux4))
                        + $R11.ml_aux8)
                    + $R11.ml_odd_aux4)
                    + (- $R11.ml_aux6))
                    + $R11.ml_odd_aux)
                    + $R11.ml_aux8)
                + (- $L11.ml_aux8))
                + $R11.ml_odd_aux2)
                + (- $R11.ml_odd_aux4))
                + (- $L11.ml_odd_aux4))
            + 1) + (- -1)
            :
            ((($R11.cl_aux4 + $R11.ml_aux8) + (- $R11.cl_aux6)) +
            (- $R11.ml_odd_aux4)) + 1
            : $R11.ml_odd_aux4;
        _s1.ml_aux4_aux5 = false;
        _s1.ml_aux5_aux5 =
            $R11.ml_aux4_aux5 ?
            $R11.s1_aux ? $R11.ml_aux5_aux3 : $L11.ml_aux10 :
            $L11.s2_aux && ($R11.s1 && $R11.s2_aux) ?
            $L11.ml_odd_aux2 + $L11.ml_aux10 : $L11.ml_aux10;
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

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/longest_odd10s.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   s1 = false; s2 = false; cl = 0; ml = 0; ml_odd = 0; s2_aux = true; s1_aux =
//   false; cl_aux2 = 0; ml_aux6 = 0; ml_odd_aux = 0; s1_aux2 = true; cl_aux4 = 0;
//   ml_aux8 = 0; ml_odd_aux2 = 0; ml_aux4_aux3 = true; ml_aux5_aux3 = 0;
//   s1_aux4 = true; cl_aux6 = 0; ml_aux10 = 0; ml_odd_aux4 = 0; ml_aux4_aux5 =
//   true; ml_aux5_aux5 = 0; }for(x in a){
//                              _s1 = {
//                                s1 = _s1.s2 && (x = 1);
//                                s2 = x = 0;
//                                cl = if _s1.s1 then _s1.cl + 1 else 0;
//                                ml =
//                                max (_s1.ml, (if _s1.s1 then _s1.cl + 1 else 0));
//                                ml_odd =
//                                if
//                                  ((max (_s1.ml,
//                                    (if _s1.s1 then _s1.cl + 1 else 0)))
//                                   % 2) = 1
//                                  then
//                                  max (_s1.ml,
//                                    (if _s1.s1 then _s1.cl + 1 else 0))
//                                  else _s1.ml_odd;
//                                s2_aux = x = 0;
//                                s1_aux = _s1.s2_aux && (x = 1);
//                                cl_aux2 =
//                                if _s1.s1_aux then _s1.cl_aux2 + 1 else 0;
//                                ml_aux6 =
//                                max (_s1.ml_aux6,
//                                  (if _s1.s1_aux then _s1.cl_aux2 + 1 else 0));
//                                ml_odd_aux =
//                                if
//                                  ((max (_s1.ml_aux6,
//                                    (if _s1.s1_aux then _s1.cl_aux2 + 1 else 0)))
//                                   % 2) = 1
//                                  then
//                                  max (_s1.ml_aux6,
//                                    (if _s1.s1_aux then _s1.cl_aux2 + 1 else 0))
//                                  else _s1.ml_odd_aux;
//                                s1_aux2 = _s1.s2 && (x = 1);
//                                cl_aux4 =
//                                if _s1.s1_aux2 then _s1.cl_aux4 + 1 else 0;
//                                ml_aux8 =
//                                max (_s1.ml_aux8,
//                                  (if _s1.s1_aux2 then _s1.cl_aux4 + 1 else 0));
//                                ml_odd_aux2 =
//                                if
//                                  ((max (_s1.ml_aux8,
//                                    (if _s1.s1_aux2 then _s1.cl_aux4 + 1 else 0)))
//                                   % 2) = 1
//                                  then
//                                  max (_s1.ml_aux8,
//                                    (if _s1.s1_aux2 then _s1.cl_aux4 + 1 else 0))
//                                  else _s1.ml_odd_aux2;
//                                ml_aux4_aux3 = _s1.ml_aux4_aux3 && _s1.s1_aux2;
//                                ml_aux5_aux3 =
//                                if _s1.ml_aux4_aux3 && _s1.s1_aux2 then
//                                  _s1.ml_aux5_aux3 + 1 else _s1.ml_aux5_aux3;
//                                s1_aux4 = _s1.s2_aux && (x = 1);
//                                cl_aux6 =
//                                if _s1.s1_aux4 then _s1.cl_aux6 + 1 else 0;
//                                ml_aux10 =
//                                max (_s1.ml_aux10,
//                                  (if _s1.s1_aux4 then _s1.cl_aux6 + 1 else 0));
//                                ml_odd_aux4 =
//                                if
//                                  ((max (_s1.ml_aux10,
//                                    (if _s1.s1_aux4 then _s1.cl_aux6 + 1 else 0)))
//                                   % 2) = 1
//                                  then
//                                  max (_s1.ml_aux10,
//                                    (if _s1.s1_aux4 then _s1.cl_aux6 + 1 else 0))
//                                  else _s1.ml_odd_aux4;
//                                ml_aux4_aux5 = _s1.ml_aux4_aux5 && _s1.s1_aux4;
//                                ml_aux5_aux5 =
//                                if _s1.ml_aux4_aux5 && _s1.s1_aux4 then
//                                  _s1.ml_aux5_aux5 + 1 else _s1.ml_aux5_aux5;
//                              }
//                              }
//                              return _s1;
// //--- Splitting predicate: (!(a(-1) = 0)) && (a(-1) = 1)
// //--- Join function ---
// join($L11,$R11) = {
//   s1 =
//   if $L11.s1 then $R11.s1_aux && $R11.s1_aux2 else $R11.s1_aux4 && $R11.s1_aux2;
//   s2 = $R11.s2;
//   cl =
//   if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//     if $L11.s2_aux then -1 else $R11.ml;
//   ml =
//   if $L11.s2 then
//     max ($R11.ml_odd_aux,
//       (if $L11.s1_aux4 then
//        (if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//         if $L11.s2_aux then -1 else $R11.ml) + $L11.ml_aux5_aux5
//        else $L11.cl_aux4))
//     else
//     max
//       ((if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//         if $L11.s2_aux then -1 else $R11.ml),
//       (if $L11.s1 then $R11.ml_aux8 else $L11.ml));
//   ml_odd =
//   if $L11.s2 then
//     max ($R11.ml_odd_aux,
//       (if $L11.s1_aux4 then
//        (if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//         if $L11.s2_aux then -1 else $R11.ml) + $L11.ml_aux5_aux5
//        else $L11.cl_aux4))
//     else
//     max
//       ((if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//         if $L11.s2_aux then -1 else $R11.ml),
//       (if $L11.s1 then $R11.ml_aux8 else $L11.ml));
//   s2_aux = $R11.s2_aux;
//   s1_aux = (¬ $R11.s2) && ($R11.s1_aux4 && $R11.s1_aux);
//   cl_aux2 = if $L11.s2 then $R11.cl_aux2 else $R11.cl_aux4;
//   ml_aux6 =
//   if $L11.s2_aux then
//     max ($R11.ml_odd_aux,
//       (if $L11.s2_aux then $L11.ml_odd_aux else $R11.ml_aux10))
//     else
//     max ($L11.ml_odd_aux,
//       (if $L11.s1_aux4 then $L11.cl + $R11.ml_aux8 else $R11.ml_odd));
//   ml_odd_aux =
//   if $L11.s2_aux then
//     max ($R11.ml_odd_aux,
//       (if $L11.s2_aux then $L11.ml_odd_aux else $R11.ml_aux10))
//     else
//     max ($L11.ml_odd_aux,
//       (if $L11.s1_aux4 then $L11.cl + $R11.ml_aux8 else $R11.ml_odd));
//   s1_aux2 = $R11.s1 && $R11.s1_aux;
//   cl_aux4 =
//   if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//     if $L11.s2_aux then -1 else $R11.ml;
//   ml_aux8 =
//   max (1,
//     (if $R11.s1_aux2 then
//      (if $L11.s2 then if $R11.s1_aux2 then 0 else $R11.cl_aux2 else
//       if $L11.s2_aux then -1 else $R11.ml) + -1
//      else 0));
//   ml_odd_aux2 =
//   if $R11.cl_aux4 = $R11.ml_aux6 then
//     if $L11.cl_aux4 = 1 then
//       ((($R11.ml_odd_aux2 + $L11.cl_aux2) + $L11.cl_aux4) + (- $L11.ml_aux8)) +
//         (- $L11.ml_odd_aux2)
//       else ((- $R11.cl_aux2) + $R11.cl_aux4) + $R11.ml_odd_aux2
//     else $R11.ml_odd_aux2;
//   ml_aux4_aux3 =
//   if $R11.s1_aux4 then $L11.ml_aux4_aux3 && ($R11.ml_aux4_aux3 && $R11.s2) else
//     $L11.ml_aux4_aux3 && $L11.ml_aux4_aux5;
//   ml_aux5_aux3 = $R11.ml_aux5_aux3;
//   s1_aux4 = if $R11.s2 then $R11.s1_aux4 && $L11.s2_aux else $R11.s1_aux4;
//   cl_aux6 =
//   if $R11.ml_aux4_aux5 then if $L11.s2_aux then $R11.ml_aux10 + -1 else $R11.cl
//     else $R11.cl_aux6;
//   ml_aux10 =
//   if $R11.ml_aux4_aux3 then
//     max ($L11.cl_aux6,
//       (if $R11.s1 then
//        (if $R11.ml_aux4_aux5 then
//         if $L11.s2_aux then $R11.ml_aux10 + -1 else $R11.cl else $R11.cl_aux6)
//          + $L11.cl
//        else -1))
//     else
//     max ($L11.ml_aux5_aux5,
//       (if $L11.ml_aux4_aux3 then
//        (if $R11.ml_aux4_aux5 then
//         if $L11.s2_aux then $R11.ml_aux10 + -1 else $R11.cl else $R11.cl_aux6)
//          + $L11.cl_aux2
//        else $R11.ml_odd_aux4));
//   ml_odd_aux4 =
//   if $R11.ml_aux10 = $R11.cl_aux4 then
//     if $L11.ml_aux10 = 1 then
//       ((((((((((((($R11.cl_aux2 + (- $R11.ml_aux6)) + (- $R11.ml_odd_aux)) +
//                  (- $R11.cl_aux4))
//                 + $R11.ml_aux8)
//                + $R11.ml_odd_aux4)
//               + (- $R11.ml_aux6))
//              + $R11.ml_odd_aux)
//             + $R11.ml_aux8)
//            + (- $L11.ml_aux8))
//           + $R11.ml_odd_aux2)
//          + (- $R11.ml_odd_aux4))
//         + (- $L11.ml_odd_aux4))
//        + 1) + (- -1)
//       else
//       ((($R11.cl_aux4 + $R11.ml_aux8) + (- $R11.cl_aux6)) +
//        (- $R11.ml_odd_aux4)) + 1
//     else $R11.ml_odd_aux4;
//   ml_aux4_aux5 = false;
//   ml_aux5_aux5 =
//   if $R11.ml_aux4_aux5 then
//     if $R11.s1_aux then $R11.ml_aux5_aux3 else $L11.ml_aux10 else
//     if $L11.s2_aux && ($R11.s1 && $R11.s2_aux) then
//       $L11.ml_odd_aux2 + $L11.ml_aux10 else $L11.ml_aux10;
// }
// // Synthesized in (predicate : 0.33 s) + (join : 130.74 s)