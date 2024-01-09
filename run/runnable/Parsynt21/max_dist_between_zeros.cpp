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
    int md = 0; int cd = 0; bool seen1 = false; bool md_aux5 = true;
    int md_aux6 = 0; int cd_aux = 0; int md_aux7 = 0; int cd_aux3 = 0;
    bool md_aux5_aux1 = true; int md_aux6_aux1 = 0;
};
result dac(int l, int r)
{
    if (l == r) {
        int x = w[l];
        result _s1;
        _s1.md = std::max(_s1.md, (_s1.seen1 || (x == 1) && (!(x == 1)) ? _s1.cd + 1 : 0));
        _s1.cd = (_s1.seen1 || (x == 1) && (!(x == 1)) ? _s1.cd + 1 : 0);
        _s1.seen1 = _s1.seen1 || (x == 1);
        _s1.md_aux5 = _s1.md_aux5 && (_s1.seen1 && (!(x == 1)));
        _s1.md_aux6 = (_s1.md_aux5 && (_s1.seen1 && (!(x == 1))) ? _s1.md_aux6 + 1 : _s1.md_aux6);
        _s1.cd_aux = 1 + _s1.cd_aux;
        _s1.md_aux7 = std::max(_s1.md_aux7, (!(x == 1) ? _s1.cd_aux3 + 1 : 0));
        _s1.cd_aux3 = (!(x == 1) ? _s1.cd_aux3 + 1 : 0);
        _s1.md_aux5_aux1 = _s1.md_aux5_aux1 && (!(x == 1));
        _s1.md_aux6_aux1 = (_s1.md_aux5_aux1 && (!(x == 1)) ? _s1.md_aux6_aux1 + 1 : _s1.md_aux6_aux1);
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
        _s1.md =
            $L7.md_aux5_aux1 ?
            std::max($R7.md, ($L7.seen1 ? $L7.md_aux7 + $R7.md_aux6_aux1 : -1))
            :
            std::max($R7.md_aux7,
            ($R7.seen1 || $L7.seen1 && (!($L7.cd == $L7.md_aux6)) ?
            $L7.md_aux7 + $R7.md_aux6_aux1 : $L7.md));
        _s1.cd =
            $L7.seen1 ?
            $R7.md_aux5_aux1 || $L7.md_aux5_aux1 && (!($R7.cd_aux3 == -1)) ?
            $L7.cd + $R7.cd_aux : $R7.cd
            :
            $L7.md_aux5 || $L7.seen1 && (!($L7.cd == $L7.md_aux7)) ?
            $L7.cd_aux3 + $L7.md_aux7 : $R7.cd;
        _s1.seen1 = $L7.seen1 || $R7.seen1;
        _s1.md_aux5 =
            (!$R7.seen1) &&
            ($L7.md_aux5_aux1 && ($L7.seen1 || $R7.seen1 && (!$R7.seen1)));
        _s1.md_aux6 = $L7.md_aux5 ? $L7.md + 1 : $L7.md_aux6;
        _s1.cd_aux = $L7.cd_aux + $R7.cd_aux;
        _s1.md_aux7 =
            std::max($R7.md_aux7,
            (!($R7.md_aux6 == $L7.cd_aux3) ? $R7.md_aux6_aux1 + $L7.md_aux7 :
            $L7.md_aux7));
        _s1.cd_aux3 =
            $R7.md_aux5 ? !($R7.md == $L7.md) ? $L7.md : $R7.cd_aux :
            $R7.md_aux5_aux1 ? $L7.cd_aux3 + $R7.cd_aux : $R7.cd_aux3;
        _s1.md_aux5_aux1 = $L7.md_aux5_aux1 && $R7.md_aux5_aux1;
        _s1.md_aux6_aux1 =
            $L7.seen1 ?
            $L7.md_aux5_aux1 ? $R7.md + $L7.md_aux6_aux1 : $L7.md_aux6_aux1
            : !($L7.cd == 1) ? $L7.cd_aux + $R7.md_aux6_aux1 : $L7.md;
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
    printf("%d\n", res.md);
    printf("%.10lf\n", cost);
}

//  SUCCESS  Parsed /root/parsynt/inputs/AutoLifter/max_dist_between_zeros.minic.
//  SUCCESS  Equation system: 
// // --- Lifted function ---
// _s1 = {
//   md = 0; cd = 0; seen1 = false; md_aux5 = true; md_aux6 = 0; cd_aux = 0;
//   md_aux7 = 0; cd_aux3 = 0; md_aux5_aux1 = true; md_aux6_aux1 = 0; }for(x in a){
//                                                                     _s1 = {
//                                                                     md =
//                                                                     max
//                                                                     (_s1.md,
//                                                                     (if
//                                                                     _s1.seen1
//                                                                     || 
//                                                                     (x = 1) &&
//                                                                     (¬
//                                                                     (x = 1))
//                                                                     then
//                                                                     _s1.cd + 1
//                                                                     else 0));
//                                                                     cd =
//                                                                     if
//                                                                     _s1.seen1
//                                                                     || 
//                                                                     (x = 1) &&
//                                                                     (¬
//                                                                     (x = 1))
//                                                                     then
//                                                                     _s1.cd + 1
//                                                                     else 0;
//                                                                     seen1 =
//                                                                     _s1.seen1
//                                                                     || 
//                                                                     (x = 1);
//                                                                     md_aux5 =
//                                                                     _s1.md_aux5
//                                                                     &&
//                                                                     (_s1.seen1
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 1)));
//                                                                     md_aux6 =
//                                                                     if
//                                                                     _s1.md_aux5
//                                                                     &&
//                                                                     (_s1.seen1
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 1)))
//                                                                     then
//                                                                     _s1.md_aux6
//                                                                     + 1 else
//                                                                     _s1.md_aux6;
//                                                                     cd_aux =
//                                                                     1 +
//                                                                     _s1.cd_aux;
//                                                                     md_aux7 =
//                                                                     max
//                                                                     (_s1.md_aux7,
//                                                                     (if
//                                                                     ¬ (x = 1)
//                                                                     then
//                                                                     _s1.cd_aux3
//                                                                     + 1 else 0));
//                                                                     cd_aux3 =
//                                                                     if
//                                                                     ¬ (x = 1)
//                                                                     then
//                                                                     _s1.cd_aux3
//                                                                     + 1 else 0;
//                                                                     md_aux5_aux1 =
//                                                                     _s1.md_aux5_aux1
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 1));
//                                                                     md_aux6_aux1 =
//                                                                     if
//                                                                     _s1.md_aux5_aux1
//                                                                     &&
//                                                                     (¬
//                                                                     (x = 1))
//                                                                     then
//                                                                     _s1.md_aux6_aux1
//                                                                     + 1 else
//                                                                     _s1.md_aux6_aux1;
//                                                                     }
//                                                                     }
//                                                                     return _s1;
// //--- Splitting predicate:
// ¬ ((¬ (a(-1) = 1)) || (a(0) = 1) && (¬ (a(0) = 1)))
// //--- Join function ---
// join($L7,$R7) = {
//   md =
//   if $L7.md_aux5_aux1 then
//     max ($R7.md, (if $L7.seen1 then $L7.md_aux7 + $R7.md_aux6_aux1 else -1))
//     else
//     max ($R7.md_aux7,
//       (if $R7.seen1 || $L7.seen1 && (¬ ($L7.cd = $L7.md_aux6)) then
//        $L7.md_aux7 + $R7.md_aux6_aux1 else $L7.md));
//   cd =
//   if $L7.seen1 then
//     if $R7.md_aux5_aux1 || $L7.md_aux5_aux1 && (¬ ($R7.cd_aux3 = -1)) then
//       $L7.cd + $R7.cd_aux else $R7.cd
//     else
//     if $L7.md_aux5 || $L7.seen1 && (¬ ($L7.cd = $L7.md_aux7)) then
//       $L7.cd_aux3 + $L7.md_aux7 else $R7.cd;
//   seen1 = $L7.seen1 || $R7.seen1;
//   md_aux5 =
//   (¬ $R7.seen1) &&
//     ($L7.md_aux5_aux1 && ($L7.seen1 || $R7.seen1 && (¬ $R7.seen1)));
//   md_aux6 = if $L7.md_aux5 then $L7.md + 1 else $L7.md_aux6;
//   cd_aux = $L7.cd_aux + $R7.cd_aux;
//   md_aux7 =
//   max ($R7.md_aux7,
//     (if ¬ ($R7.md_aux6 = $L7.cd_aux3) then $R7.md_aux6_aux1 + $L7.md_aux7 else
//      $L7.md_aux7));
//   cd_aux3 =
//   if $R7.md_aux5 then if ¬ ($R7.md = $L7.md) then $L7.md else $R7.cd_aux else
//     if $R7.md_aux5_aux1 then $L7.cd_aux3 + $R7.cd_aux else $R7.cd_aux3;
//   md_aux5_aux1 = $L7.md_aux5_aux1 && $R7.md_aux5_aux1;
//   md_aux6_aux1 =
//   if $L7.seen1 then
//     if $L7.md_aux5_aux1 then $R7.md + $L7.md_aux6_aux1 else $L7.md_aux6_aux1
//     else if ¬ ($L7.cd = 1) then $L7.cd_aux + $R7.md_aux6_aux1 else $L7.md;
// }
// // Synthesized in (predicate : 0.33 s) + (join : 35.30 s)