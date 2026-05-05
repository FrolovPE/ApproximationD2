
#include <QPainter>
#include <stdio.h>
#include <cmath>
#include <cstring>
#include <algorithm>

#include "window.h"
#include "msr.h"
#include "args.h"
#include "mytime.h"

#define DEFAULT_A   -1.0
#define DEFAULT_B    1.0
#define DEFAULT_C   -1.0
#define DEFAULT_D    1.0
#define DEFAULT_NX   5
#define DEFAULT_NY   5
#define DEFAULT_MX   100
#define DEFAULT_MY   100
#define DEFAULT_K    0
#define DEFAULT_MODE 0
#define DEFAULT_SCALE 0
#define DEFAULT_P    0

static const char *func_names[] = {
    "k=0 f(x,y)=1",
    "k=1 f(x,y)=x",
    "k=2 f(x,y)=y",
    "k=3 f(x,y)=x+y",
    "k=4 f(x,y)=sqrt(x^2+y^2)",
    "k=5 f(x,y)=x^2+y^2",
    "k=6 f(x,y)=exp(x^2-y^2)",
    "k=7 f(x,y)=1/(25(x^2+y^2)+1)"
};

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

static double (*func_table[])(double, double) = {
    f_0, f_1, f_2, f_3, f_4, f_5, f_6, f_7
};
static const int N_FUNCS = 8;

static QColor palette_color(double t)
{
    t = std::max(0.0, std::min(1.0, t));
    int hue = (int)((1.0 - t) * 240);
    return QColor::fromHsv(hue, 255, 255);
}

Window::Window(QWidget *parent)
    : QWidget(parent),
      tid(nullptr), ap(nullptr), argv0(nullptr),
      A(nullptr), B(nullptr), I(nullptr), coeff(nullptr), coeff_work(nullptr),
      r(nullptr), u(nullptr), v(nullptr), sp(nullptr),
      vals(nullptr), vals_size(0)
{
    a = DEFAULT_A; b = DEFAULT_B;
    c = DEFAULT_C; d = DEFAULT_D;
    nx = DEFAULT_NX; ny = DEFAULT_NY;
    mx = DEFAULT_MX; my = DEFAULT_MY;
    eps = 1e-10; mi = 1000; p = 1;
    k = DEFAULT_K;
    draw_mode = DEFAULT_MODE;
    mode_name = "MODE I f(x,y)";
    scale = DEFAULT_SCALE;
    P = DEFAULT_P;
    it = 0;

    set_ready(READY);
    has_res = false;
    connect(&timer, SIGNAL(timeout()), this, SLOT(check_recompute()));
    timer.start(50);

    a0 = a; b0 = b; c0 = c; d0 = d;
    func_id = k;
    set_func();
    // realloc();
    // eval();
}

Window::~Window()
{
    timer.stop();
    if (tid != nullptr) {
        for (int thr = 0; thr < p; thr++)
            pthread_join(tid[thr], nullptr);
        delete[] tid; tid = nullptr;
        delete[] ap;  ap  = nullptr;
    }
    delete[] A;
    delete[] B;
    delete[] I;
    delete[] coeff;
    delete[] coeff_work;
    delete[] r;
    delete[] u;
    delete[] v;
    delete[] sp;
    delete[] vals;
    pthread_mutex_destroy(&mutex);
}

QSize Window::minimumSizeHint() const { return QSize(100, 100); }
QSize Window::sizeHint()        const { return QSize(1000, 1000); }

int Window::parse_command_line(int argc, char *argv[])
{
    if (argc == 1) return 0;
    if (argc != 13) return -1;

    if (   sscanf(argv[1],  "%lf", &a)   != 1
        || sscanf(argv[2],  "%lf", &b)   != 1
        || sscanf(argv[3],  "%lf", &c)   != 1
        || sscanf(argv[4],  "%lf", &d)   != 1
        || sscanf(argv[5],  "%d",  &nx)  != 1
        || sscanf(argv[6],  "%d",  &ny)  != 1
        || sscanf(argv[7],  "%d",  &mx)  != 1
        || sscanf(argv[8],  "%d",  &my)  != 1
        || sscanf(argv[9],  "%d",  &k)   != 1
        || sscanf(argv[10], "%lf", &eps) != 1
        || sscanf(argv[11], "%d",  &mi)  != 1
        || sscanf(argv[12], "%d",  &p)   != 1
        || b - a < 1e-6 || d - c < 1e-6
        || nx <= 0 || ny <= 0
        || mx <= 0 || my <= 0
        || p <= 0 || k < 0 || k >= N_FUNCS)
        return -2;

    argv0 = argv[0];
    a0 = a; b0 = b; c0 = c; d0 = d;
    func_id = k;
    set_func();
    realloc();
    eval();
    return 0;
}

void Window::set_func()
{
    f = func_table[func_id];
    f_name = func_names[func_id];
    update();
}

void Window::change_func()
{
    if (!can_recompute())
        return;

    func_id = (func_id + 1) % N_FUNCS;
    set_func();
    // realloc();
    eval();
}

void Window::set_mode()
{
    switch (draw_mode) {
        case 0: mode_name = "MODE I f(x,y)";      break;
        case 1: mode_name = "MODE II Pf(x,y)";     break;
        case 2: mode_name = "MODE III |f-Pf|(x,y)"; break;
    }
    update();
}

void Window::change_mode()
{
    draw_mode = (draw_mode + 1) % 3;
    set_mode();
}

void Window::realloc()
{
    delete[] A; delete[] B; delete[] I;
    delete[] coeff; delete[] coeff_work; delete[] r; delete[] u; delete[] v; delete[] sp;
    A = nullptr; B = nullptr; I = nullptr;
    coeff = nullptr; coeff_work = nullptr; r = nullptr; u = nullptr; v = nullptr; sp = nullptr;

    int n = (nx+1)*(ny+1);
    int N = n + get_len_msr(nx, ny) + 1;

    A     = new double[N];
    I     = new int[N];
    B     = new double[n];
    coeff = new double[n];
    coeff_work = new double[n];
    r     = new double[n];
    u     = new double[n];
    v     = new double[n];
    sp    = new double[p];

    memset(coeff, 0, n * sizeof(double));
    memset(coeff_work, 0, n * sizeof(double));
    memset(B,     0, n * sizeof(double));
    memset(r,     0, n * sizeof(double));
    memset(u,     0, n * sizeof(double));
    memset(v,     0, n * sizeof(double));

    has_res = false;
}

double Window::max_f()
{
    double maxf = 0;
    int N = 1000;
    double hx = (b0 - a0) / N;
    double hy = (d0 - c0) / N;
    for (int i = 0; i <= N; i++)
        for (int j = 0; j <= N; j++) {
            double val = fabs(f(a0 + i*hx, c0 + j*hy));
            if (val > maxf) maxf = val;
        }
    return maxf;
}

void Window::eval()
{

    if (!can_recompute())
        return;

    set_ready(BUSY);

    int n = (nx+1)*(ny+1);
    int N = n + get_len_msr(nx, ny) + 1;
    

    memset(coeff, 0, n * sizeof(double));
    memset(B,     0, n * sizeof(double));
    memset(r,     0, n * sizeof(double));
    memset(u,     0, n * sizeof(double));
    memset(v,     0, n * sizeof(double));

    double mf = max_f();

    delete[] ap;   ap  = nullptr;
    delete[] tid;  tid = nullptr;

    ap  = new args[p];
    tid = new pthread_t[p];
    
    

    for (int thr = 0; thr < p; thr++) {
        ap[thr].thr  = thr; 
        ap[thr].p = p;
        ap[thr].mutex = &mutex;
        ap[thr].ready = &ready;
        ap[thr].argv0 = argv0;


        ap[thr].a = a0; 
        ap[thr].b = b0;
        ap[thr].c = c0;
        ap[thr].d = d0;

        ap[thr].nx = nx; 
        ap[thr].ny = ny;
        ap[thr].k  = func_id;
        ap[thr].eps = eps; 
        ap[thr].mi = mi;
        ap[thr].n = n; 
        ap[thr].N = N;

        ap[thr].A = A; 
        ap[thr].I = I;
        ap[thr].B = B; 
        ap[thr].x = coeff_work;
        ap[thr].u = u; 
        ap[thr].v = v;
        ap[thr].r = r; 
        ap[thr].sp = sp;

        ap[thr].it = &it;
        ap[thr].P = P;
        ap[thr].p_maxf = mf;
    }

    for (int thr = 0; thr < p; thr++)
        pthread_create(tid + thr, nullptr, ApproxD2, ap + thr);

    // msr_sovle(ap + 0);

    // for (int thr = 1; thr < p; thr++)
    //     pthread_join(tid[thr], nullptr);

    

    update();
}

void Window::change_ab()
{
    double mid_x = (a0 + b0) / 2;
    double mid_y = (c0 + d0) / 2;
    double hx = (b0 - a0) * std::pow(2.0, (double)scale);
    double hy = (d0 - c0) * std::pow(2.0, (double)scale);
    a = mid_x - hx / 2;
    b = mid_x + hx / 2;
    c = mid_y - hy / 2;
    d = mid_y + hy / 2;
}

void Window::paintEvent(QPaintEvent *)
{
    //esli zanyati ne schitat'
    if (!has_res)
    {
        QPainter painter(this);
        painter.drawText(20, 30, "WAIT, THREADS ARE BUSY");
        return;
    }


    QPainter painter(this);
    int W = width(), H = height();
    if (W <= 0 || H <= 0 || !coeff) return;

    double hx_v = (b - a) / mx;
    double hy_v = (d - c) / my;

    int ntri = 2 * mx * my;
    if (ntri != vals_size) {
        delete[] vals;
        vals = new double[ntri];
        vals_size = ntri;
    }

    double vmin =  1e30;
    double vmax = -1e30;

    for (int j = 0; j < my; j++) {
        for (int i = 0; i < mx; i++) {
            double xi = a + i * hx_v;
            double yj = c + j * hy_v;

            // centri mass verhnego i nijnego tri
            double xi1 = xi + 2.0/3.0 * hx_v, yj1 = yj + 1.0/3.0 * hy_v;
            double xi2 = xi + 1.0/3.0 * hx_v, yj2 = yj + 2.0/3.0 * hy_v;

            double v1, v2;
            if (draw_mode == 0) {
                v1 = f(xi1, yj1);
                v2 = f(xi2, yj2);
            } else if (draw_mode == 1) {
                v1 = Pf(xi1, yj1, a0, b0, c0, d0, nx, ny, coeff);
                v2 = Pf(xi2, yj2, a0, b0, c0, d0, nx, ny, coeff);
            } else {
                v1 = fabs(f(xi1,yj1) - Pf(xi1,yj1, a0,b0,c0,d0, nx,ny,coeff));
                v2 = fabs(f(xi2,yj2) - Pf(xi2,yj2, a0,b0,c0,d0, nx,ny,coeff));
            }

            int idx = 2*(j*mx + i);
            vals[idx]   = v1;
            vals[idx+1] = v2;
            if (v1 < vmin) vmin = v1;
            if (v1 > vmax) vmax = v1;
            if (v2 < vmin) vmin = v2;
            if (v2 > vmax) vmax = v2;
        }
    }

    double range = (vmax > vmin) ? (vmax - vmin) : 1.0;
    double fmax_display = std::max(fabs(vmin), fabs(vmax));

    painter.setPen(Qt::NoPen);

    for (int j = 0; j < my; j++) {
        for (int i = 0; i < mx; i++) {
            double xi = a + i * hx_v;
            double yj = c + j * hy_v;

            double sx00 = (xi - a) / (b - a) * W;
            double sx10 = (xi + hx_v - a) / (b - a) * W;
            double sy00 = (d - yj) / (d - c) * H;
            double sy01 = (d - (yj+hy_v)) / (d - c) * H;

            QPointF P00(sx00, sy00);
            QPointF P10(sx10, sy00);
            QPointF P01(sx00, sy01);
            QPointF P11(sx10, sy01);

            int idx = 2*(j*mx + i);

            QPointF tri1[3] = {P00, P10, P11};
            painter.setBrush(palette_color((vals[idx] - vmin) / range));
            painter.drawPolygon(tri1, 3);

            QPointF tri2[3] = {P00, P01, P11};
            painter.setBrush(palette_color((vals[idx+1] - vmin) / range));
            painter.drawPolygon(tri2, 3);
        }
    }

    char txt[256];
    QPen pen_red(Qt::black, 1);
    painter.setPen(pen_red);

    snprintf(txt, sizeof(txt), "%s  %s", f_name, mode_name);
    painter.drawText(5, 20, txt);
    snprintf(txt, sizeof(txt), "Nx=%d Ny=%d  Mx=%d My=%d", nx, ny, mx, my);
    painter.drawText(5, 40, txt);
    snprintf(txt, sizeof(txt), "scale=%d  P=%d eps = %10.3e", scale, P,eps);
    painter.drawText(5, 60, txt);
    snprintf(txt, sizeof(txt), "max{|Fmin|,|Fmax|}=%e", fmax_display);
    painter.drawText(5, 80, txt);

    printf("max{|Fmin|,|Fmax|}=%e\n", fmax_display);
}

void Window::resize_mult()
{
    if (!can_recompute())
        return;

    nx *= 2; ny *= 2;
    realloc();
    eval();
}

void Window::resize_dev()
{
    if (!can_recompute())
        return;


    nx = std::max(2, nx / 2);
    ny = std::max(2, ny / 2);
    realloc();
    eval();
}

void Window::rescale_mult()
{
    scale++;
    change_ab();
    update();
}

void Window::rescale_dev()
{
    scale--;
    change_ab();
    update();
}

void Window::add_p()
{
    if (!can_recompute())
        return;


    P++;
    eval();
}

void Window::sub_p()
{
    if (!can_recompute())
        return;


    P--;
    eval();
}

void Window::resize_viz_mult()
{
    mx *= 2; my *= 2;
    update();
}

void Window::resize_viz_dev()
{
    mx = std::max(2, mx / 2);
    my = std::max(2, my / 2);
    update();
}

bool Window::can_recompute() 
{
    int value;

   
    value = get_ready();
    

    return value == READY;

}

void Window::check_recompute()
{
    if (get_ready() != DONE)
        return;

    for (int thr = 0; thr < p; thr++)
    {
        pthread_join(tid[thr], nullptr);
    }

    delete[] ap;
    delete[] tid;
    

    tid = nullptr;
    ap = nullptr;

    std::swap(coeff, coeff_work);

    has_res = true;

    set_ready(READY);

    if (close_requested) {
        close();
        return;
    }

    update();
}

int Window::get_ready()
{
    int value;

    pthread_mutex_lock(&mutex);
    value = ready;
    pthread_mutex_unlock(&mutex);

    return value;
}

void Window::set_ready(int value)
{
    pthread_mutex_lock(&mutex);
    ready = value;
    pthread_mutex_unlock(&mutex);
}


void Window::closeEvent(QCloseEvent *event)
{
    if (!can_recompute()) {
        close_requested = true;
        event->ignore();
        return;
    }
    event->accept();
}

void Window::check_n_close() // zamena close
{
    if (!can_recompute())
        return;
    
    close();
}
