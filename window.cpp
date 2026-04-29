
#include <QPainter>
#include <stdio.h>

#include "window.h"
#include "newton.h"
#include "spline.h"

#define DEFAULT_A -1
#define DEFAULT_B 1
#define DEFAULT_N 5
#define DEFAULT_K 0
#define DEFAULT_MODE 0
#define DEFAULT_SCALE 0
#define DEFAULT_P 0

#define L2G(X,Y) (l2g ((X), (Y), min_y, max_y))

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

static void make_xy(int n, double a, double b, double *x, double *y, double (*func)(double))
{
    double h = (b-a)/n;

    for(int i = 0 ; i < n; i++)
    {
        x[i] = a + i*h;
        y[i] = func(x[i]);
    }
}



Window::Window (QWidget *parent)
  : QWidget (parent),x(nullptr),y(nullptr),coeff(nullptr),tmp(nullptr)
{
  a = DEFAULT_A;
  b = DEFAULT_B;
  n = DEFAULT_N;
  k = DEFAULT_K;

  draw_mode = DEFAULT_MODE;
  mode_name = "mode I";
  scale = DEFAULT_SCALE;
  P = DEFAULT_P;

  a0 = a;
  b0 = b;

 

  func_id = k;

  set_func();

  realloc ();
  eval ();





  // change_func ();
  
}

Window::~Window()
{
  if(A)
    delete []A;
  if(B)
    delete []B;
  if(I)
    delete []I;
  if(coeff)
    delete []coeff;
  if(tmp)
    delete []tmp;
  if(r)
    delete []r;
  if(u)
    delete []u;
  if(v)
    delete []v;
  if(sp)
    delete []sp;
}

QSize Window::minimumSizeHint () const
{
  return QSize (100, 100);
}

QSize Window::sizeHint () const
{
  return QSize (1000, 1000);
}

void Window::realloc ()
{
  if(x)
    delete []x;
  if(y)
    delete []y;
  if(coeff)
    delete []coeff;
  if(tmp)
    delete []tmp;

  // x = new double[n];
  // y = new double[n];
  coeff = new double[4*n];
  tmp = new double[7*(n + 1)];

  
}

int Window::parse_command_line (int argc, char *argv[])
{
  if (argc == 1)
    return 0;

  if (argc == 2)
    return -1;

  if (   sscanf (argv[1], "%lf", &a) != 1
      || sscanf (argv[2], "%lf", &b) != 1
      || sscanf (argv[3], "%lf", &c) != 1
      || sscanf (argv[4], "%lf", &d) != 1
      || b - a < 1.e-6
      || d - c < 1.e-6
      || (argc > 3 && (sscanf (argv[5], "%d", &nx) != 1))
      || (argc > 4 && (sscanf (argv[6], "%d", &ny) != 1))
      || (argc > 5 && (sscanf (argv[7], "%d", &mx) != 1))
      || (argc > 6 && (sscanf (argv[8], "%d", &my) != 1))
      || (argc > 7 && (sscanf (argv[9], "%d", &k) != 1))
      || (argc > 8 && (sscanf (argv[10], "%lf", &eps) != 1))
      || (argc > 9 && (sscanf (argv[11], "%d", &mi) != 1))
      || (argc > 10 && (sscanf (argv[12], "%d", &p) != 1))
      || nx <= 0      || ny <= 0
      || mx <= 0      || my <= 0
      || p <= 0
      ||(k < 0 || k > 6)
      )
    return -2;
  // printf("A = %lf B = %lf N = %d K = %d\n",a,b,n,k);

  func_id = k;
  set_func();

  a0 = a;
  b0 = b;

  realloc ();
  
  eval ();
  // printf("A = %lf B = %lf N = %d K = %d func_id = %d\n",a,b,n,k,func_id);

  return 0;
}

/// change current function for drawing
void Window::change_func ()
{
  // printf("func_id = %d\n",func_id);
  func_id = (func_id + 1) % 7;

  set_func();
  realloc();
  eval();
}

void Window::change_mode()
{
  draw_mode = (draw_mode + 1)%4;
  set_mode();
}

void Window::set_mode()
{
  switch (draw_mode)
    {
      case 0:
        mode_name = "Mode I";
        break;
      case 1:
        mode_name = "Mode II";
        break;
      case 2:
        mode_name = "Mode III";
        break;
      case 3:
        mode_name = "Mode IV";
        break;
      
    }
    update ();
}

void Window::set_func ()
{
 
  switch (func_id)
    {
      case 0:
        f_name = "k = 0 f (x) = 1";
        f = f_0;
        break;
      case 1:
        f_name = "k = 1 f (x) = x";
        f = f_1;
        break;
      case 2:
      f_name = "k = 2 f (x) = x * x";
      f = f_2;
      break;
      case 3:
        f_name = "k = 3 f (x) = x * x * x";
        f = f_3;
        break;
      case 4:
        f_name = "k = 4 f (x) = x * x * x * x";
        f = f_4;
        break;
      case 5:
      f_name = "k = 5 f (x) = exp(x)";
      f = f_5;
      break;
      case 6:
        f_name = "k = 6 f (x) = 1/(25 * x * x + 1)";
        f = f_6;
        break;
    }
  update ();
}

QPointF Window::l2g (double x_loc, double y_loc, double y_min, double y_max)
{
  double x_gl = (x_loc - a) / (b - a) * width ();
  double y_gl = (y_max - y_loc) / (y_max - y_min) * height ();
  return QPointF (x_gl, y_gl);
}

void Window::eval ()
{


  make_xy(n,a0,b0,x,y,f);

  newton_tmp = tmp;
  newton_cff = coeff;

  spline_tmp = newton_tmp + n;
  spline_cff = newton_cff + n;


  double maxf = max_f();

  do_p(y,maxf);

  //Newton
  
  if(n <= 50)
    newton_coeff(n,x,y,newton_cff,newton_tmp);
  

  //Spline

  spline_coeff(n,x,y,spline_cff,spline_tmp);

  // printf("IM HERE\n");



  ksi_append(n,x,spline_tmp);
}

void Window::draw_graph(QPainter &painter, int width,int n,double a, double b,double min_y,double max_y,double delta_y, double (*func)(double, int,double* ,double* ,double*),double *x, double *coeff,double *tmp)
{
  double x1, x2, y1, y2;
  // double max_y, min_y;
  // double delta_y;

  double hx = (b - a)/width;

  // max_y = min_y = 0;


  // for (int i = 0; i <= width; i++)
  //   {
  //     x1 = a + i * hx;
  //     y1 = func(x1,n,x,coeff,tmp);
  //     if (y1 < min_y)
  //       min_y = y1;
  //     if (y1 > max_y)
  //       max_y = y1;
  //   }

  // delta_y = 0.01 * (max_y - min_y)/2;
  // min_y -= delta_y;
  // max_y += delta_y;


  printf("\nmax{|Fmin|,|Fmax|} = %lf\n",std::max(fabs(min_y),fabs(max_y))-delta_y);

  x1 = a;
  y1 = func(x1,n,x,coeff,tmp);

  for (int i = 1; i <= width; i++)
    {
      x2 = a + i * hx;
      y2 = func(x2,n,x,coeff,tmp);
      // local coords are converted to draw coords
      painter.drawLine (L2G(x1, y1), L2G(x2, y2));

      x1 = x2, y1 = y2;
    }
}

void Window::draw_error(QPainter &painter, int width,int n,double a, double b, double &min_y,double &max_y,
  double (*func)(double,int, double*, double*, double*),double (*f)(double),
  double *x, double *coeff,double *tmp)
{

  double x1, x2, y1, y2;
  // double max_y, min_y;
   double delta_y;

  double hx = (b - a)/width;

   max_y = min_y = 0;


   for (int i = 0; i <= width; i++)
     {
       x1 = a + i * hx;
       y1 = f(x1) -func(x1,n,x,coeff,tmp);
       if (y1 < min_y)
         min_y = y1;
       if (y1 > max_y)
         max_y = y1;
     }

   delta_y = 0.01 * std::max(max_y - min_y,1e-15);
   min_y -= delta_y;
   max_y += delta_y;




  

  x1 = a;
  y1 = f(x1) -func(x1,n,x,coeff,tmp);

  for (int i = 1; i <= width; i++)
    {
      x2 = a + i * hx;
      y2 = f(x2) -func(x2,n,x,coeff,tmp);
      // local coords are converted to draw coords
      painter.drawLine (L2G(x1, y1), L2G(x2, y2));

      x1 = x2, y1 = y2;
    }

}


/// render graph
void Window::paintEvent (QPaintEvent * /* event */)
{  
  QPainter painter (this);
  double x1, x2, y1, y2;
  double max_y, min_y;
  double delta_y;
  // Line for graph: green, line width 2, solid
  QPen pen_green(Qt::green, 2, Qt::SolidLine);
  QPen pen_blue(Qt::blue, 3, Qt::SolidLine);
  QPen pen_orange(Qt::gray, 3, Qt::SolidLine);
  // Line for axes: red, line width 1, solid
  QPen pen_red(Qt::red, 1, Qt::SolidLine);
  // Window width
  int W = width (), i;
  // Step
  double hx = (b - a) / W;

  

  

  painter.setPen (pen_green);

  // calculate min and max for current function
  max_y = min_y = 0;
  for (i = 0; i <= W; i++)
    {
      x1 = a + i * hx;
      y1 = f (x1);
      if (y1 < min_y)
        min_y = y1;
      if (y1 > max_y)
        max_y = y1;
    }

  delta_y = 0.01 * (max_y - min_y);
  min_y -= delta_y;
  max_y += delta_y;

  char txt[50];

  
  // draw approximated line for graph
  if(draw_mode != 3)
  {x1 = a;
  y1 = f (x1);
  for (i = 1; i <= W; i++)
    {
      x2 = a + i * hx;
      y2 = f (x2);
      // local coords are converted to draw coords
      painter.drawLine (L2G(x1, y1), L2G(x2, y2));

      x1 = x2, y1 = y2;
    }
      painter.setPen (pen_blue);
      snprintf(txt,sizeof(txt),"\nmax{|Fmin|,|Fmax|} = %lf\n",std::max(fabs(min_y ),fabs(max_y)) - delta_y);
      painter.drawText (0, 100, txt);
  }
  // draw axis
  painter.setPen (pen_red);
  painter.drawLine (L2G(a, 0), L2G(b, 0));
  painter.drawLine (L2G(0, min_y), L2G(0, max_y));

  

  snprintf(txt,sizeof(txt),"%s     %s       ",f_name,mode_name);
  // render function name
  painter.setPen (pen_red);
  painter.drawText (0, 20, txt);
  snprintf(txt,sizeof(txt),"p = %d",P);
  painter.drawText (50, 60, txt);
  snprintf(txt,sizeof(txt),"n = %d",n);
  painter.drawText (0, 40, txt);
  snprintf(txt,sizeof(txt),"s = %d",scale);
  painter.drawText (0, 60, txt);
  snprintf(txt,sizeof(txt),"a = %lf    b = %lf",a,b);
  painter.drawText (0, 80, txt);
  painter.setPen (pen_blue);

  
  // printf("IN parintEvent\n");
  

  //alloc memory

 

  // double *newton_tmp = tmp;
  // double *newton_cff = coeff;

  // double *spline_tmp = newton_tmp + n;
  // double *spline_cff = newton_cff + n;
  
  
  

  


  if(draw_mode == 0 && n <= 50) 
    draw_graph(painter,W,n,a,b,min_y, max_y, delta_y,&newton_in_point,x,newton_cff,newton_tmp);
  else if(draw_mode == 1)
    {
      painter.setPen (pen_orange);
      draw_graph(painter,W,n,a,b,min_y, max_y,delta_y,&spline_in_point,x,spline_cff,spline_tmp);
    }
  else if(draw_mode == 2)
    {
      if(n <= 50) 
        draw_graph(painter,W,n,a,b,min_y, max_y,delta_y,&newton_in_point,x,newton_cff,newton_tmp);

      painter.setPen (pen_orange);
      draw_graph(painter,W,n,a,b,min_y, max_y,delta_y,&spline_in_point,x,spline_cff,spline_tmp);
    }
  else if(draw_mode == 3)
    {  

      double maxx{};
      double maxx_y{},minn_y{};
      if(n <= 50) 
        draw_error(painter,W,n,a,b,maxx_y,minn_y,&newton_in_point,f,x,newton_cff,newton_tmp);

      if(maxx_y < EPS)
           maxx_y = EPS;
      if(minn_y < EPS)
           minn_y = EPS;

      maxx = std::max(fabs(minn_y ),std::max(maxx,fabs(maxx_y)));



      painter.setPen (pen_orange);
      draw_error(painter,W,n,a,b,maxx_y,minn_y,&spline_in_point,f,x,spline_cff,spline_tmp);

      if(maxx_y < EPS)
           maxx_y = EPS;
      if(minn_y < EPS)
           minn_y = EPS;

      maxx = std::max(fabs(minn_y ),std::max(maxx,fabs(maxx_y)));

      painter.setPen (pen_red);
      snprintf(txt,sizeof(txt),"\nmax{|Fmin|,|Fmax|} = %10.3e\n",maxx);
      painter.drawText (0, 100, txt);
      printf("\nmax{|Fmin|,|Fmax|} = %10.3e\n",maxx);
    }

  //delete memory



}


void Window::resize_mult ()
{
  n *= 2;
  realloc();
  eval ();
  update();
}


void Window::resize_dev ()
{
  n /= 2;
  n = (n <= 1 ? 2:n);
  realloc();
  eval ();
  update();
}


void Window::rescale_mult ()
{
  // scale = 1;
  scale++;
  change_ab ();
  // eval ();
  update ();
}


void Window::rescale_dev ()
{
  // scale = -1;
  scale--;
  change_ab ();
  // eval ();
  update ();
}

void Window::change_ab ()
{
  double mid = (a0 + b0) / 2;

  double h = (b0 - a0) * std::pow(2,scale);

  a = mid - h / 2;
  b = mid + h / 2;
}

double Window::max_f()
{
  double x,maxf{},fabsf;
  int size = 5000;

  double h = (b - a) / size;

  for(int i = 0; i < size; i++)
  {
    x = a + i * h;
    fabsf = fabs(f(x));
    if(maxf < fabsf)
      maxf = fabsf;

  }

  return maxf;
}

void Window::do_p (double *y,double maxf)
{
  int mid = n / 2;

  y[mid] += P * maxf * 0.1;
}


void Window::add_p ()
{
  // p = 0;
  P++;
  eval ();
  update ();
}
void Window::sub_p ()
{
  // p = 0;
  P--;
  eval ();
  update ();
}
