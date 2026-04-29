#ifndef MSR_H
#define MSR_H
#include <stdio.h>
#include <pthread.h>
#include <cstring>
#include "args.h"
#include "extpll.h"
#include "mytime.h"

#define EPS 1e-16
#define MAXPRINT 7
#define MAXSTEPS 1000000

void *parallelMSR(void *arg);
void get_my_rows(int n, int p, int thr, int &i1, int &i2);
void report(const char *name, int task, double r1, double r2, double r3, double r4, double t1, double t2,int it, double eps, int k, int nx, int ny, int p);
int get_len_msr(int nx, int ny);
void* msr_sovle(void* ptr);
void ij2l(int nx, int ny, int i, int j, int &l);
void l2ij(int nx, int ny, int l, int &i, int &j);
int IA_ij(int nx, int ny, double hx, double hy, int i, int j, int is, int js, int s, int *I = nullptr, double *A = nullptr);
int get_diag(int nx, int ny, double hx, double hy, int i, int j,int* /*I*/, double *A);
int get_off_diag(int nx, int ny, double hx, double hy, int i, int j, int *I, double *A);
void fill_I_diag(int nx, int ny, double hx, double hy, int *I);
int fill_IA(int nx, int ny, double hx, double hy, int *I, double *A, int p, int k);
int init_msr_matrix(int nx,int ny, double hx, double hy, int *I, double *A,int p, int k);
void print_msr_matrix(double *A, int *I, int n, int nn);
int check_symm(int nx, int ny, double hx, double hy, int *I, double *A, int p, int k);
int check_row_sum(int nx, int ny, double hx, double hy, int *I,double *A, int p, int k);
int get_triangle(int nx, int ny, int i, int j);
double get_aij(double *A, int *I, int i, int j);
void apply_preconditioner(double *A, int *I,int n, double *v, double *r, double *u, int p, int thr);
double res3(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) );
double res4(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) );
double res1(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) );
double res2(double a, double b, double c, double d, int nx, int ny, double *x, double (*f) (double, double) );
#endif