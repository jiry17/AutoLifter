#include <cstdio>
#include <sys/time.h>
#include <omp.h>
const int N=10000000;
int w[N + 10];
int n, lim;
double difftime(timeval start, timeval end) {
    long long second_diff = end.tv_sec - start.tv_sec;
    if (second_diff < 0) second_diff += 24 * 3600;
    long long u_diff = end.tv_usec - start.tv_usec;
    return second_diff + u_diff * 1e-6;
}
struct result {
    int x0,x1,x2;
};
result dac(int l, int r)
{
    if (l == r) {
        int a = w[l];
        return (result){0,(((1)<(a))&&((a)<=((1)+(1))))?(1000000000):(a),(((1)<(a))&&((a)<=((1)+(1))))?(1000000000):(a)};
    } else {
        result lres, rres;
        int mid = l + r >> 1;
#pragma omp task shared(lres) if(r - l > lim)
        lres = dac(l, mid);
#pragma omp task shared(rres) if(r - l > lim)
        rres = dac(mid + 1, r);
#pragma omp taskwait
        return (result){(((lres.x2)<(rres.x1))&&((rres.x1)!=(1000000000)))?(((lres.x0)+(rres.x0))+(1)):((lres.x0)+(rres.x0)),((lres.x1)==(1000000000))?(rres.x1):(lres.x1),((rres.x2)==(1000000000))?(lres.x2):(rres.x2)};
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
    printf("%d\n", res.x0);
    printf("%.10lf\n", cost);
}