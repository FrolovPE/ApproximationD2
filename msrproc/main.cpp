#include <iostream>
#include "msr.h"

int main(int argc, char** argv) {
    

    int TASK = 7;
    double a,b,c,d;
    int nx,ny;
    int k;
    double eps;
    int mi;
    int p;
    double r1=0, r2=0, r3=0, r4=0;
    double t1=0, t2=0;
    int it = 0;

    if(! ( argc == 11 && (sscanf(argv[1], "%lf", &a ) == 1) && (sscanf(argv[2], "%lf", &b) == 1) && (sscanf(argv[3], "%lf", &c) == 1) && (sscanf(argv[4], "%lf", &d) == 1) && (sscanf(argv[5], "%d", &nx) == 1) && (sscanf(argv[6], "%d", &ny) == 1) && (sscanf(argv[7], "%d", &k) == 1) && (sscanf(argv[8], "%lf", &eps) == 1)  && (sscanf(argv[9], "%d", &mi) == 1) && (sscanf(argv[10], "%d", &p) == 1)) )
    {
        printf("Usage: %s a b c d nx ny k eps mi p\n", argv[0]);
        return 0;
    }

    // printf("a = %lf b = %lf c = %lf d = %lf nx = %d ny = %d k = %d eps = %lf mi = %d p = %d\n", a, b, c, d, nx, ny, k, eps, mi, p);
    

    if(nx <= 0 || ny <= 0 || k < 0 || eps <= 0 || mi <= 0 || p <= 0)
    {
        printf("Wrong usage! nx, ny, k, eps, mi and p must be > 0\n");
        return 0;
    }

    int n = (nx+1) * (ny+1);
    int N = n + get_len_msr(nx, ny) + 1;

    double *A;
    int *I;
    double *B,*x,*u,*v,*r;

    A = A; I = I; n = n;

    A = new double[N];
    I = new int[N];

    B = new double[n];
    x = new double[n];
    u = new double[n];
    v = new double[n];
    r = new double[n];

    for(int i = 0 ; i < n; i++)
    {
        B[i] = 0;
        x[i] = 0;
        u[i] = 0;
        v[i] = 0;
        r[i] = 0;
    }

    args *ap = new args[p];
    pthread_t *tid = new pthread_t[p];
    double *sp = new double[p];

    int thr;

    for(thr = 0; thr < p; thr ++)
    {
        ap[thr].thr = thr;
        ap[thr].p = p;
        ap[thr].a = a;
        ap[thr].b = b;
        ap[thr].c = c;
        ap[thr].d = d;
        ap[thr].nx = nx;
        ap[thr].ny = ny;
        ap[thr].k = k;
        ap[thr].eps = eps;
        ap[thr].mi = mi;
        ap[thr].n = n;
        ap[thr].N = N;
        ap[thr].A = A;
        ap[thr].I = I;
        ap[thr].B = B;
        ap[thr].x = x;
        ap[thr].u = u;
        ap[thr].v = v;
        ap[thr].r = r;
        ap[thr].sp = sp;
        ap[thr].it = &it;
        

    }

    t1 = get_full_time();

    for(thr = 1; thr < p; thr++)
    {
        if(pthread_create(tid+thr,0,msr_sovle,ap+thr))
        {
            printf("ERROR: Cannot create thread %d\n",thr);
            delete []A;
            delete []I;
            delete []ap;
            delete []tid;
            delete []B;
            delete []x;
            delete []u;
            delete []v;
            delete []r;
            delete []sp;
           

            return -1;
        }
    }

    void* ret0 = msr_sovle(ap + 0);
    void* ret;

    ret0 = ret0;
    
    for(thr = 1; thr < p; thr++)
    {
        if(pthread_join(tid[thr], &ret))
        {
            printf("ERROR: Cannot join thread %d\n",thr);
            delete []A;
            delete []I;
            delete []ap;
            delete []tid;
            delete []B;
            delete []x;
            delete []u;
            delete []v;
            delete []r;
            delete []sp;
            return -1;
        }
    }

    t1 = get_full_time() - t1;

    t2 = get_full_time();
    //Residuals
    r1 = res1(a, b, c, d, nx, ny, x, ap[0].f);
    r2 = res2(a, b, c, d, nx, ny, x, ap[0].f);
    r3 = res3(a, b, c, d, nx, ny, x, ap[0].f);
    r4 = res4(a, b, c, d, nx, ny, x, ap[0].f);

    t2 = get_full_time() - t2;
    
    report(argv[0],TASK,r1,r2,r3,r4,t1,t2,it,eps,k,nx,ny,p);

    delete []A;
    delete []I;
    delete []ap;
    delete []tid;
    delete []B;
    delete []x;
    delete []u;
    delete []v;
    delete []r;
    delete []sp;

    return 0;
}
