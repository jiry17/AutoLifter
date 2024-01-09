#include <cassert>
#include <cstdio>
#include <sys/time.h>
double difftime(timeval start, timeval end) {
    long long second_diff = end.tv_sec - start.tv_sec;
    if (second_diff < 0) second_diff += 24 * 3600;
    long long u_diff = end.tv_usec - start.tv_usec;
    return second_diff + u_diff * 1e-6;
}
int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a < b ? b : a;
}
const int N = 10000000;
int n, m;
int w[N + 10], tag[(N << 2) + 10];
struct result {
    int x0,x1;
}info[(N << 2) + 10];
void apply(int pos, int a) {
    result res = info[pos];
    tag[pos] = tag[pos]?0:1;
    info[pos] = (result){(0)-(res.x1),(0)-(res.x0)};
}
void pushdown(int pos) {
    if (tag[pos] == 0) return;
    apply(pos * 2, tag[pos]);
    apply(pos * 2 + 1, tag[pos]);
    tag[pos] = 0;
}
void init(int pos, int l, int r) {
    tag[pos] = 0;
    if (l == r) {
        int a = w[l]; info[pos] = (result){a,a};
        return;
    }
    int mid = l + r >> 1;
    init(pos * 2, l, mid);
    init(pos * 2 + 1, mid + 1, r);
    result lres = info[pos * 2], rres = info[pos * 2 + 1];
    info[pos] = (result){((lres.x0)<=(rres.x0))?(lres.x0):(rres.x0),((lres.x1)<=(rres.x1))?(rres.x1):(lres.x1)};
}
void update(int pos, int l, int r, int ul, int ur, int a) {
    if (l > ur || r < ul) return;
    if (l >= ul && r <= ur) {
        apply(pos, a); return;
    }
    int mid = l + r >> 1; pushdown(pos);
    update(pos * 2, l, mid, ul, ur, a);
    update(pos * 2 + 1, mid + 1, r, ul, ur, a);
    result lres = info[pos * 2], rres = info[pos * 2 + 1];
    info[pos] = (result){((lres.x0)<=(rres.x0))?(lres.x0):(rres.x0),((lres.x1)<=(rres.x1))?(rres.x1):(lres.x1)};
}
result query_res;
bool is_empty;
void query(int pos, int l, int r, int ul, int ur) {
    if (l > ur || r < ul) return;
    if (l >= ul && r <= ur) {
        if (is_empty) {
            is_empty = false; query_res = info[pos];
        } else {
            result lres = query_res, rres = info[pos];
            query_res = (result){((lres.x0)<=(rres.x0))?(lres.x0):(rres.x0),((lres.x1)<=(rres.x1))?(rres.x1):(lres.x1)};
        }
        return;
    }
    int mid = l + r >> 1; pushdown(pos);
    query(pos * 2, l, mid, ul, ur);
    query(pos * 2 + 1, mid + 1, r, ul, ur);
}
int ty[N + 10], l[N + 10], r[N + 10], val[N + 10];
int main() {
    scanf("%d%d\n",&n, &m);
    for (int i = 1; i <= n; ++i) scanf("%d", &w[i]);
    for (int i = 1; i <= m; ++i) {
        scanf("%d%d%d", &ty[i], &l[i], &r[i]);
        if (ty[i]) scanf("%d",&val[i]);
    }
    timeval start;
    gettimeofday(&start, NULL);
    init(1, 1, n); int checksum = 0;
    for (int i = 1; i <= m; ++i) {
         if (ty[i] == 0) {
             update(1, 1, n, l[i], r[i], val[i]);
         } else {
             is_empty = true;
             query(1, 1, n, l[i], r[i]);
             checksum ^= query_res.x0;
         }
    }
    timeval end;
    gettimeofday(&end, NULL);
    double cost = difftime(start, end);
    printf("%d\n", checksum);
    printf("%.10lf\n", cost);
}