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
    int x0,x1,x2,x3,x4,x5;
};
result dac(int l, int r)
{
    if (l == r) {
        int a = w[l];
        return (result){a,a,a,a,a,a};
    } else {
        result lres, rres;
        int mid = l + r >> 1;
#pragma omp task shared(lres) if(r - l > lim)
        lres = dac(l, mid);
#pragma omp task shared(rres) if(r - l > lim)
        rres = dac(mid + 1, r);
#pragma omp taskwait
        return (result){((((lres.x0)<(rres.x0))&&(((rres.x3)*(lres.x4))<(rres.x0)))&&(((rres.x1)*(lres.x2))<(rres.x0)))?(rres.x0):(((((rres.x3)*(lres.x4))<(lres.x0))&&(((rres.x1)*(lres.x2))<(lres.x0)))?(lres.x0):((((rres.x1)*(lres.x2))<=((rres.x3)*(lres.x4)))?((rres.x3)*(lres.x4)):((rres.x1)*(lres.x2)))),(((lres.x1)<=((lres.x3)*(rres.x3)))&&((lres.x5)<(0)))?((lres.x3)*(rres.x3)):((((lres.x1)<=((lres.x1)*(rres.x1)))&&((lres.x1)<=(lres.x5)))?((lres.x1)*(rres.x1)):(lres.x1)),(((rres.x2)<=((lres.x4)*(rres.x4)))&&((rres.x5)<(0)))?((lres.x4)*(rres.x4)):((((rres.x2)<=((lres.x2)*(rres.x2)))&&((1)<=(rres.x5)))?((lres.x2)*(rres.x2)):(rres.x2)),(((1)<=(lres.x5))&&(((lres.x1)*(rres.x3))<=(lres.x3)))?((lres.x1)*(rres.x3)):(((((rres.x1)*(lres.x3))<=(lres.x3))&&((lres.x5)<=(lres.x3)))?((rres.x1)*(lres.x3)):(lres.x3)),((((rres.x5)*(lres.x4))<=(rres.x4))&&((1)<=(rres.x5)))?((rres.x5)*(lres.x4)):((((rres.x5)<=(rres.x4))&&(((lres.x2)*(rres.x4))<=(rres.x4)))?((lres.x2)*(rres.x4)):(rres.x4)),(lres.x5)*(rres.x5)};
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