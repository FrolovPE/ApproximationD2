#ifndef ARGS_H
#define ARGS_H
#include <cmath>

class args
{
    public:
        double *A;
        int *I;
        double *B, *x, *u, *v,*r;
        double *sp;
        int p;
        int thr;
        int a, b, c, d;
        int nx, ny, k, mi;
        int n,N;
        double eps;
        double (*f)(double, double);
        double *t1;
        int *it;

    void choose_func(int k)
    {
        switch(k)
        {
            case 0:
                f = f_0;
                break;
            case 1:
                f = f_1;
                break;
            case 2:
                f = f_2;
                break;
            case 3:
                f = f_3;
                break;
            case 4:
                f = f_4;
                break;
            case 5:
                f = f_5;
                break;
            case 6:
                f = f_6;
                break;
            case 7:
                f = f_7;
                break;
            default:
                f = f_0;
                break;
        }
    }

    private:
    static double f_0(double x, double y)
    {
        x=x; y=y;
        return 1;
    }

    static double f_1(double x, double y)
    {
        y=y;
        return x;
    }

    static double f_2(double x, double y)
    {
        x=x;
        return y;
    }

    static double f_3(double x, double y)
    {
        return x+y;
    }

    static double f_4(double x, double y)
    {
        return sqrt(x*x+y*y);
    }
    static double f_5(double x, double y)
    {
        return (x*x+y*y);
    }

    static double f_6(double x, double y)
    {
        return std::exp(x*x - y * y);
    }

    static double f_7(double x, double y)
    {
        return 1.0 / ( 25 * (x*x+y*y) + 1);
    }

};





#endif