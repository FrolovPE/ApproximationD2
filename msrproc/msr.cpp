#include "msr.h"
#define F(IS, JS, S) (IA_ij(nx, ny, hx, hy, i, j, (IS), (JS), (S), I, A))

void report(const char *name, int task, double r1, double r2, double r3, double r4, double t1, double t2,int it, double eps, int k, int nx, int ny, int p)
{
    printf (
        "%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
        name, task, r1, r2, r3, r4, t1, t2, it, eps, k, nx, ny, p);
}

void get_my_rows(int n, int p, int thr, int &i1, int &i2)
{
    i1 = n * thr; i1 /= p;
    i2 = n * (thr + 1); i2 /= p;   
}

int get_len_msr(int nx, int ny)
{
    return (nx - 1) * (ny - 1) * 6 + (nx - 1) * 4 * 2 + (ny - 1) * 4 * 2 + 3 * 2 + 2 * 2;
}
void ij2l(int nx, int ny, int i, int j, int &l)
{
    ny = ny;
    l = i + j * (nx + 1);
}
void l2ij(int nx, int ny, int l, int &i, int &j)
{
    (void)ny;
    j = l / (nx + 1);
    i = l - j * (nx + 1);
}

int IA_ij(int nx, int ny, double hx, double hy, int i, int j, int is, int js, int s, int *I, double *A)
{
    double S = 0.5 * hx * hy;
    int l, ls;

    ij2l(nx, ny, i, j, l);
    ij2l(nx, ny, is, js, ls);

    if (l != ls)
        I[s] = ls;

    if(!A && !I)
    {
        printf("A and I are nullptrs in IA_ij for i = %d , j = %d , is = %d , js = %d , s = %d\n",i,j,is,js,s);
        return -1;
    }

    if(i > 0 && i < nx && j > 0 && j < ny)
    {
        if (l  == ls) A[s] = 6 * S / 6;
        else A[s] = 2 * S / 12;
    }

    if(i > 0 && i < nx && j == 0)
    {
        if (l == ls) A[s] = 3 * S / 6;
        else if (s == 0 || s == 1) A[s] = 1 * S / 12;
        else if (s == 2 || s == 3) A[s] = 2 * S / 12;
        else return -1;
    }

    if(i > 0 && i < nx && j == ny)
    {
        if (l == ls) A[s] = 3 * S / 6;
        else if (s == 0 || s == 3) A[s] = 1 * S / 12;
        else if (s == 1 || s == 2) A[s] = 2 * S / 12;
        else return -1;
    }

    if(i == 0 && j > 0 && j < ny)
    {
        if (l == ls) A[s] = 3 * S / 6;
        else if (s == 0 || s == 3) A[s] = 2 * S / 12;
        else if (s == 1 || s == 2) A[s] = 1 * S / 12;
        else return -1;
    }

    if(i == nx && j > 0 && j < ny)
    {
        if (l == ls) A[s] = 3 * S / 6;
        else if (s == 0 || s == 3) A[s] = 1 * S / 12;
        else if (s == 1 || s == 2) A[s] = 2 * S / 12;
        else return -1;
    }

    if(i == 0 && j == 0)
    {
        if (l == ls) A[s] = 2 * S / 6;
        else if (s == 0 || s == 1) A[s] = 1 * S / 12;
        else if (s == 2) A[s] = 2 * S / 12;
        else return -1;
    }

    if(i == nx && j == ny)
    {
        if (l == ls) A[s] = 2 * S / 6;
        else if (s == 0 || s == 2) A[s] = 1 * S / 12;
        else if (s == 1) A[s] = 2 * S / 12;
        else return -1;
    }

    if(i == 0 && j == ny)
    {
        if (l == ls) A[s] = 1 * S / 6;
        else if (s == 0 || s == 1) A[s] = 1 * S / 12;
        else return -1;
    }

    if(i == nx && j == 0)
    {
        if (l == ls) A[s] = 1 * S / 6;
        else if (s == 0 || s == 1) A[s] = 1 * S / 12;
        else return -1;
    }

    return 0;
}

int get_diag(int nx, int ny, double hx, double hy, int i, int j,int* /*I*/, double *A)
{
    return IA_ij(nx, ny, hx, hy, i, j, i, j, 0,nullptr, A);
}

int get_off_diag(int nx, int ny, double hx, double hy, int i, int j, int *I, double *A)
{
    if(i > 0 && i < nx && j > 0 && j < ny)
    {
        if (I && A)
        {
            F(i + 1, j, 0), F(i, j - 1, 1), F(i - 1, j - 1, 2), F(i - 1, j, 3), F(i, j + 1, 4), F(i + 1, j + 1, 5);
        }
        return 6;
    }

    if (i > 0 && i < nx && j == 0)
    {
        if (I && A)
        {
            F(i + 1, j, 0), F(i - 1, j, 1), F(i, j + 1, 2), F(i + 1, j + 1, 3);
        }
        return 4;
    }


    if (i > 0 && i < nx && j == ny)
    {
        if (I && A)
        {
            F(i + 1, j, 0), F(i, j - 1, 1), F(i - 1, j - 1, 2), F(i - 1, j, 3);
        }
        return 4;
    }
    
    
    if (i == 0 && j > 0 && j < ny)
    {
        if (I && A)
        {
            F(i + 1, j, 0), F(i, j - 1, 1), F(i, j + 1, 2), F(i + 1, j + 1, 3);
        }
        return 4;
    }
    
    
    if (i == nx && j > 0 && j < ny)
    {
        if (I && A)
        {
            F(i, j - 1, 0), F(i - 1, j - 1, 1), F(i - 1, j,2 ), F(i ,j + 1 ,3);
        }
        return 4;
    }
    
    
    if (i == 0 && j == 0)
    {
        if (I && A)
        {
            F(i + 1 ,j ,0 ),F( i ,j+1 ,1 ),F( i+1 ,j+1 ,2 );
        }
        return 3;
    }

    if ( i == nx && j == ny)
    {
        if (I && A)
        {
            F(i ,j - 1 ,0 ),F( i - 1 ,j - 1 ,1 ),F( i - 1 ,j ,2 );
        }
        return 3;
    }

    if( i == 0 && j == ny)
    {
        if (I && A)
        {
            F(i + 1 ,j ,0 ),F( i ,j - 1 ,1 );
        }
        return 2;
    }

    if (i == nx && j == 0)
    {
        if (I && A)
        {
            F(i - 1,j  ,0 ),F( i  ,j + 1 ,1 );
        }
        return 2;
    }

    printf("Wrong i and j in get_off_diag: i = %d , j = %d\n",i,j);
    return -999999999;
}

void fill_I_diag(int nx, int ny, double hx, double hy, int *I)
{
    int N = (nx + 1) * (ny + 1),i,j,l,r = N + 1, s;

    for(l = 0 ; l < N; l++)
    {
        l2ij(nx,ny,l,i,j);
        // printf("l = %d , i = %d , j = %d\n",l,i,j);
        s = get_off_diag(nx, ny, hx, hy, i, j, nullptr, nullptr);
        I[l] = r;
        r += s;
    }

    I[l] = r;
}

int fill_IA(int nx, int ny, double hx, double hy, int *I, double *A, int p, int thr)
{
    int i,j,l,l1,l2,s,r,t,err=0,len=0;
    int N = (nx + 1) * (ny + 1);
    get_my_rows(N, p, thr, l1, l2);
    // printf("Thread %d is filling rows from %d to %d\n",thr,l1,l2);
    for(l = l1; l < l2; l++)
    {
        r = I[l];
        s = I[l + 1] - I[l];

        l2ij(nx, ny, l, i, j);
        // printf("Thread %d is filling row %d (i = %d , j = %d)\n",thr,l,i,j);
        if(get_diag(nx, ny, hx, hy, i, j, nullptr, A + l))
        {
            err = -1;
            break;
        }
        
        t = get_off_diag(nx, ny, hx, hy, i, j, I + r, A + r);
        if(s != t)
        {
            err = -2;
            break;
        }
        len += s;
    }
    reduce_sum(p, &err, 1);
    if(err < 0)
        return -1;
    reduce_sum(p, &len, 1);
    if(I[N] != len + N + 1)
        return -2;
    return 0;
}

int get_triangle(int nx, int ny, int i, int j)
{
    if (i > 0 && i < nx && j > 0 && j < ny)
        return 6;
    
    if ((i == 0 || i == nx) && j > 0 && j < ny)
        return 3;
    
    if (i > 0 && i < nx && (j == 0 || j == ny))
        return 3;
    
     if ((i == 0 && j == 0) || (i == nx && j == ny))
        return 2;

    if ((i == 0 && j == ny) || (i == nx && j == 0))
        return 1;
  
    return -999999;
}

int check_row_sum(int nx, int ny, double hx, double hy, int *I,double *A, int p, int thr)
{
    int l,l1,l2,r,t,m,i,j,err=0;
    double sum;
    int N = (nx + 1) * (ny + 1);
    get_my_rows(N, p, thr, l1, l2);

    for(l = l1; l < l2; l++)
    {
        r = I[l + 1] - I[l];
        m = I[l];
        sum = A[l];
        for(t = 0; t < r; t++)
        {
            sum += A[m + t];
        }
        l2ij(nx, ny, l, i, j);
        int count = get_triangle(nx, ny, i, j);
        if(fabs(sum - count * hx * hy / 6) > EPS)
        {
            printf("sum = %e , count * hx * hy / 6 = %e , diff = %e , prec = %e\n",sum,count * hx * hy / 6,fabs(sum - count * hx * hy / 6),EPS);
            err = -1;
            break;
        }
    }
    reduce_sum(p, &err, 1);
    return err;
}

int check_symm(int nx, int ny,double hx, double hy, int *I, double *A, int p, int thr)
{
    int l,l1,l2,r,t,m,j,err=0;
    int N = (nx + 1) * (ny + 1);
    double alj;
    int tj;


    get_my_rows(N, p, thr, l1, l2);
    for(l = l1; l < l2; l++)
    {
        r = I[l + 1] - I[l];
        m = I[l];
        for(t = 0; t < r; t++)
        {
            j = I[m + t];
            alj = A[m + t];
            int rj = I[j + 1] - I[j];
            int mj = I[j];
            
            for(tj = 0; tj < rj; tj++)
            {
                if(I[mj + tj] == l)
                    break;
                
            }

            if(tj >= rj) 
            {
                err = -1;
                break;
            }

            if(fabs(alj - A[mj + tj]) > EPS * hx * hy)
            {
                err = -2;
                break;
            }

            
            
        
        }

        if(err < 0) break;
        
    }
    reduce_sum(p, &err, 1);
    return err;
}

double get_aij(double *A,int *I,int i , int j)
{
    if(i == j)
        return A[i];
    
    int l1 = I[i], l2 = I[i + 1];
    for(int l = l1; l < l2; l++)
    {
        if(I[l] == j)
            return A[l];
    }
    return 0;
}

void print_msr_matrix(double *A, int *I, int n, int maxprint)
{
    int nn = (n < maxprint ? n : maxprint);
    for(int i = 0; i < nn; i++)
    {
        for(int j = 0; j < nn; j++)
        {
            printf(" %10.3e", get_aij(A, I, i, j));
        }
        printf("\n");
    }
}

int init_msr_matrix(int nx,int ny, double hx, double hy, int *I, double *A,int p, int thr)
{
   int err = 0;
   if(thr == 0)
   {
        fill_I_diag(nx, ny, hx, hy, I);
   }
   reduce_sum(p);

   err = fill_IA(nx, ny, hx, hy, I, A, p, thr);
   if(err < 0)
   {
        if(thr == 0)
            printf("ERROR: fill_IA failed\n");
        return -1;
   }

   return 0;


}
#undef F
#define F(I,J) (f(x0 + (I)*hx, y0 + (J) * hy))
double F_ij(int nx, int ny, double hx, double hy, double x0, double y0, int i, int j, double (*f) (double, double))
{
    double w = hx * hy /192;

    if(i > 0 && i < nx && j > 0 && j < ny)
    {
        return w * (
            36 * F(i,j) +

            20 * ( F(i + 0.5 , j) + F(i , j + 0.5) + F(i - 0.5 , j) + 
            F(i - 0.5 , j - 0.5) + F(i , j - 0.5) + F(i + 0.5 , j + 0.5) ) + 

            4 * ( F(i + 0.5 , j - 0.5) + F(i - 0.5 , j - 1) + F(i - 1 , j - 0.5) + 
            F(i - 0.5 , j + 0.5) + F(i + 0.5 , j + 1) + F(i + 1 , j + 0.5) ) +

            2 * (F(i + 1 , j) + F(i, j - 1) + F(i - 1 , j) + F(i, j + 1) + F(i + 1 , j + 1) + F(i - 1 , j - 1))
        );
    }

    if(i > 0 && i < nx && j == 0)
    {
        return w * (
            18 * F(i , j) +

            10 * ( F(i + 0.5 , j) + F(i - 0.5, j) ) + 

            20 * ( F(i , j + 0.5) + F(i + 0.5 , j + 0.5) ) +

            4 * ( F(i + 1 , j + 0.5) + F(i - 0.5 , j + 0.5) + F(i + 0.5 , j + 1) ) + 

            2 * ( F(i, j + 1) + F(i + 1 , j + 1) ) +

            F(i + 1 , j) + F(i - 1 , j)
        );
    }

    if(i > 0 && i < nx && j == ny)
    {
        return w * (
            18 * F(i , j) +

            10 * ( F(i + 0.5 , j) + F(i - 0.5, j) ) + 

            20 * ( F(i , j - 0.5) + F(i - 0.5 , j - 0.5) ) +

            4 * ( F(i + 0.5 , j - 0.5) + F(i - 0.5 , j - 1) + F(i - 1 , j - 0.5) ) + 

            2 * ( F(i, j - 1) + F(i - 1 , j - 1) ) +

            F(i + 1 , j) + F(i - 1 , j)
        );
    }

    if(i == 0 && j > 0 && j < ny)
    {
        return w * (
            18 * F(i , j) +

            10 * ( F(i , j + 0.5) + F(i, j - 0.5) ) + 

            20 * ( F(i + 0.5, j + 0.5) + F(i + 0.5 , j) ) +

            4 * ( F(i + 0.5 , j - 0.5) + F(i + 1 , j + 0.5) + F(i + 0.5 , j + 1) ) + 

            2 * ( F(i + 1 , j + 1) + F(i + 1 , j) ) +

            F(i , j + 1) + F(i , j - 1)
        );
    }

    if(i == nx && j > 0 && j < ny)
    {
        return w * (
            18 * F(i , j) +

            10 * ( F(i , j - 0.5) + F(i, j + 0.5) ) + 

            20 * ( F(i - 0.5, j - 0.5) + F(i - 0.5 , j) ) +

            4 * ( F(i - 0.5 , j + 0.5) + F(i - 1 , j - 0.5) + F(i - 0.5 , j - 1) ) + 

            2 * ( F(i - 1 , j - 1) + F(i - 1 , j) ) +

            F(i , j + 1) + F(i , j - 1)
        );
    }

    if(i == 0 && j == 0)
    {
        return w * (
            12 * F(i , j) +

            10 * ( F(i + 0.5, j) + F(i, j + 0.5) ) + 

            20 * ( F(i + 0.5, j + 0.5) ) +

            4 * ( F(i + 0.5 , j + 1) + F(i + 1 , j + 0.5) ) + 

            2 * ( F(i + 1 , j + 1) ) +

            F(i , j + 1) + F(i + 1, j)
        );
    }

    if(i == nx && j == ny)
    {
        return w * (
            12 * F(i , j) +

            10 * ( F(i - 0.5, j) + F(i, j - 0.5) ) + 

            20 * ( F(i - 0.5, j - 0.5) ) +

            4 * ( F(i - 0.5 , j - 1) + F(i - 1 , j - 0.5) ) + 

            2 * ( F(i - 1 , j - 1) ) +

            F(i , j - 1) + F(i - 1, j)
        );
    }

    if(i == 0 && j == ny)
    {
        return w * (
            6 * F(i , j) +

            10 * ( F(i + 0.5, j) + F(i, j - 0.5) ) + 

            4 * ( F(i + 0.5 , j - 0.5)  ) + 

            F(i , j - 1) + F(i + 1, j)
        );
    }

    if(i == nx && j == 0)
    {
        return w * (
            6 * F(i , j) +

            10 * ( F(i - 0.5, j) + F(i, j + 0.5) ) + 

            4 * ( F(i - 0.5 , j + 0.5)  ) + 

            F(i , j + 1) + F(i - 1, j)
        );
    }

    return -99999999;

}

void fill_B(int nx, int ny, double a, double b, double c, double d, double *B, double (*f) (double ,double), int p, int thr)
{
    int l1,l2,l,i,j;
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;

    int N = (nx + 1) * (ny + 1);

    get_my_rows(N,p,thr,l1,l2);

    for(l = l1; l < l2; l++)
    {
        l2ij(nx,ny,l,i,j);
        B[l] = F_ij(nx,ny,hx,hy,a,c,i,j,f);
    }
    reduce_sum(p);
}

void print_array(double *a, int n, int maxprint)
{
    int nn = (n < maxprint ? n : maxprint);

    for (int i = 0; i < nn; i++)
    {
        printf(" %10.3e", a[i]);
    }
    printf("\n");
}

double scalar_product(int n, double *x, double *y, int p, int thr,double *sp/*arr len p*/)
{
    int i,i1,i2;
    double s = 0;

    get_my_rows(n, p, thr, i1, i2);

    for(i = i1; i < i2; i++)
    {
        s += x[i] * y[i];
    }
    sp[thr] = s;
    reduce_sum(p);
    
    s = 0;
    for(i = 0; i < p; i++)
    {
        s += sp[i];
    }
    reduce_sum(p);
    return s;
}

void mult_sub_vector(int n, double *x, double *y, double t, int p, int thr)
{
    int i,i1,i2;

    get_my_rows(n, p, thr, i1, i2);

    for(i = i1; i < i2; i++)
    {
        x[i] -= t * y[i];
    }
    reduce_sum(p);
}

void matrix_mult_vector_msr(double *A, int *I, int n, double *x, double *b, int p, int thr)
{
    int i,i1,i2,j,J,l;
    double s;
    if(thr == 0)
        memset(b, 0, n * sizeof(double));
    reduce_sum(p);

    get_my_rows(n, p, thr, i1, i2);

    for(i = i1; i < i2; i++)
    {
        s = A[i] * x[i];
        l = I[i + 1] - I[i];
        J = I[i];
        for(j = 0; j < l; j++)
        {
            s += A[J + j] * x[I[J + j]];
        }
        b[i] = s;
    }
    reduce_sum(p);
}

int solve_minimal_error(int n, double *A, int *I,double *B, double *x, double *r, double *u, double *v, double eps,int maxit, int p, int thr,double *sp)
{
    (void)A; (void)I; (void)B; (void)x; (void)u; (void)v; (void)r; (void)p; (void)thr; (void)eps; (void)maxit;
    double c1,c2,prec,b_norm,tau;
    int it;

    b_norm = sqrt(scalar_product(n, B, B,p,thr,sp));
    prec = eps * eps * b_norm;

    matrix_mult_vector_msr(A,I,n,x,r,p,thr);
    mult_sub_vector(n, r, B, 1, p, thr);

    for(it = 0; it < maxit; it++)
    {
        apply_preconditioner(A,I,n,v,r,u,p,thr);
        matrix_mult_vector_msr(A,I,n,v,u,p,thr);
        c1 = scalar_product(n, r, v, p, thr, sp);
        c2 = scalar_product(n, u, v, p, thr, sp);

        if (c1 < prec || c2 < prec)
        {
            break;
        }

        tau = c1 / c2;
        mult_sub_vector(n, x, v, tau, p, thr);
        mult_sub_vector(n, r, u, tau, p, thr);
    }

    int err = 0;

    if(it >= maxit)
    {
        // if(thr == 0)
        //     printf("Warning: Maximum number of iterations reached !\n");
        err = -1;
    }

    reduce_sum(p, &err, 1);
    if(err < 0)
        return -1;  
        
    return it;
}

int solve_minimal_error_full(int n, double *A, int *I,double *B, double *x, double *r, double *u, double *v, double eps,int maxit, int maxsteps, int p, int thr, double *sp)
{
    int step,ret,its = 0;
    

    for(step = 0; step < maxsteps; step++)
    {
        ret = solve_minimal_error(n, A, I, B, x, r, u, v, eps, maxit, p, thr, sp);
        if(ret >= 0)
        {
            its += ret; break;
        }
        its += maxit;
    }

    if (step >= maxsteps)
    {
        return -1;
    }

    return its;
}
// solve_msr_full
void apply_preconditioner(double *A, int *I,int n, double *v, double *r, double *u, int p, int thr)
{
    //(D + wL)^t * D^{-1} * (D + wL) * v = r

    double w = 1;

    int i,i1,i2,k1,k2;
    if(thr == 0)
        memset(u, 0, n * sizeof(double));

    reduce_sum(p);
    A = A; I = I; n = n; v = v; r = r; u = u; p = p; thr = thr;

    get_my_rows(n, p, thr, i1, i2);

    if(thr == 0)
    {    
        // Solve (D + wL) * u = r
        for(i = 0; i < n; i++)
        {
            double s = r[i];
            s=s;
            w=w;
            k1 = I[i];
            k2 = I[i + 1];
            for(int k = k1; k < k2; k++)
            {
                int j = I[k];
                // printf("i = %d , j = %d, k = %d\n",i,j,k);
                if(j < i)
                {
                    s -= w * A[k] * u[j];
                }
            }
            u[i] = s / A[i];
        }

        //Solve D^{-1} * u' = u
        for(i = 0; i < n; i++)
            u[i] *= A[i];


        //Solve (D + wL)^t * v = u
        for(i = n - 1; i >= 0; i--)
        {
            double s = u[i];
            k1 = I[i];
            k2 = I[i + 1];
            for(int k = k1; k < k2; k++)
            {
                int j = I[k];
                if(j > i)
                {
                    s -= w * A[k] * v[j];
                }
            }
            v[i] = s / A[i];
        }
    }

    reduce_sum(p);

}

void* msr_sovle(void* ptr)
{
    args *arg = (args*) ptr;

    double a = arg->a;
    double b = arg->b;
    double c = arg->c;
    double d = arg->d;
    int nx = arg->nx;
    int ny = arg->ny;
    int k = arg->k;
    double eps = arg->eps;
    int mi = arg->mi;
    int p = arg->p;
    int thr = arg->thr;
    int n = arg->n;

    double *A = arg->A;
    int *I = arg->I;
    double *B = arg->B;
    double *x = arg->x;
    double *u = arg->u;
    double *v = arg->v;
    double *r = arg->r;
    double *sp = arg->sp;
    // int N = arg->N;

    arg->choose_func(k);

    double (*f)(double, double) = arg->f;

    a = a; b = b; c = c; d = d; nx = nx; ny = ny; k = k; eps = eps; mi = mi; p = p; thr = thr; A = A; I = I;x= x; u = u; v = v; f = f;
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;

    if(init_msr_matrix(nx, ny, hx, hy, I, A, p, thr) < 0)
    {
        printf("Failed to initialize MSR matrix!\n");
        return nullptr;
    }
    // reduce_sum(p);

    if (thr == 0) 
    {
        printf("Matrix A:\n");

        print_msr_matrix(A, I, n, MAXPRINT);
    }

    if(check_symm(nx, ny, hx, hy, I, A, p, thr) < 0)
    {
        if(thr == 0)
            printf("Matrix is not symmetric!\n");
        return nullptr;
    }

    if(check_row_sum(nx, ny, hx, hy, I, A, p, thr) < 0)
    {
        if(thr == 0)
            printf("Error: Incorrect row sum!\n");
        return nullptr;
    }

    fill_B(nx, ny, a, b, c, d, B, f, p, thr);

    if(thr == 0)
    {
        printf("RHS B:\n");
        print_array(B, n, MAXPRINT);
    }

    

    int it = solve_minimal_error_full(n, A, I, B, x, r, u, v, eps, mi, MAXSTEPS,p, thr, sp);
    
    if(it < 0)
    {
        if(thr == 0)
            printf("Failed to solve the system! it = %d\n", it);
        
    }

    if(thr == 0)
    {
        printf("Solution x:\n");
        print_array(x, n, MAXPRINT);
    }

    if(thr == 0)
        *arg->it = it;


    return nullptr;
}


double res3(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) )
{
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;
    double max = 0;

    for(int i = 0; i <= nx; i++)
    {
        for(int j = 0; j <= ny; j++)
        {
            double xi = a + i * hx;
            double yj = c + j * hy;
            double val = f(xi, yj);
            int l = i + j * (nx + 1);
            double r3 = fabs(x[l] - val);

            if(r3 > max)
                max = r3;

        }
    }

    return max;
}

double res4(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) )
{
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;
    double sum = 0;


    for(int i = 0; i <= nx; i++)
    {
        for(int j = 0; j <= ny; j++)
        {
            double xi = a + i * hx;
            double yj = c + j * hy;
            double val = f(xi, yj);
            int l = i + j * (nx + 1);
            double r4 = fabs(x[l] - val);

            sum += r4 * hx * hy;

        }
    }

    return sum;
} 

double res1(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) )
{
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;
    double r1 = 0;

    for(int i = 0; i < nx; i++)
    {
        for(int j = 0; j < ny; j++)
        {
            int l00, l10, l01, l11;
            ij2l(nx, ny, i, j, l00);
            ij2l(nx, ny, i + 1, j, l10);
            ij2l(nx, ny, i, j + 1, l01);
            ij2l(nx, ny, i + 1, j + 1, l11);

            double xi = a + i * hx;
            double yj = c + j * hy;
            
            double xi1 = xi + 2.0 * hx / 3;
            double yj1 = yj + 1.0 * hy / 3;

            double x00 = x[l00];
            double x10 = x[l10];
            double x01 = x[l01];
            double x11 = x[l11];

            double pf1 =  (x00 + x10 + x11)/3.0;
            double e1 = fabs(f(xi1, yj1) - pf1);

            if(e1 > r1)
                r1 = e1;

            double xi2 = xi + 1.0 * hx / 3;
            double yj2 = yj + 2.0 * hy / 3;
            double pf2 =  (x00 + x01 + x11)/3.0;
            double e2 = fabs(f(xi2, yj2) - pf2);

            if(e2 > r1)
                r1 = e2;

        }
    }
    return r1;
}

double res2(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) )
{
    double hx = (b - a) / nx;
    double hy = (d - c) / ny;
    double r2 = 0;

    for(int i = 0; i < nx; i++)
    {
        for(int j = 0; j < ny; j++)
        {
            int l00, l10, l01, l11;
            ij2l(nx, ny, i, j, l00);
            ij2l(nx, ny, i + 1, j, l10);
            ij2l(nx, ny, i, j + 1, l01);
            ij2l(nx, ny, i + 1, j + 1, l11);

            double xi = a + i * hx;
            double yj = c + j * hy;
            
            double xi1 = xi + 2.0 * hx / 3;
            double yj1 = yj + 1.0 * hy / 3;

            double x00 = x[l00];
            double x10 = x[l10];
            double x01 = x[l01];
            double x11 = x[l11];

            double pf1 =  (x00 + x10 + x11)/3.0;
            double e1 = fabs(f(xi1, yj1) - pf1);

            r2 += e1 * hx * hy * 0.5;

            double xi2 = xi + 1.0 * hx / 3;
            double yj2 = yj + 2.0 * hy / 3;
            double pf2 =  (x00 + x01 + x11)/3.0;
            double e2 = fabs(f(xi2, yj2) - pf2);

            r2 += e2 * hx * hy * 0.5;

        }
    }
    return r2;
}