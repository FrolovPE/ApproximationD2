#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>

#define EPS 1e-15

class Window : public QWidget
{
  Q_OBJECT

private:
  int func_id;
  const char *f_name;
  double a;
  double b;
  double c;
  double d;
  int nx, ny;
  int mx,my;
  double eps;
  int mi;
  int p; // threads


  double a0;
  double b0;
  int k;
  double (*f) (double,double);
  int draw_mode;
  const char *mode_name;
  int scale;
  int P;

  double *A;
  double *B;
  int *I;
  double *coeff;
  double *tmp;
  double *r,*u,*v,*sp;



public:
  Window (QWidget *parent);

  ~Window();

  QSize minimumSizeHint () const;
  QSize sizeHint () const;

  int parse_command_line (int argc, char *argv[]);
  QPointF l2g (double x_loc, double y_loc, double y_min, double y_max);
  void draw_graph(QPainter &painter, int width,int n,double a, double b,double min_y,double max_y,double delta_y, double (*func)(double,int, double*, double*, double*),double *x, double *coeff,double *tmp);
  void draw_error(QPainter &painter, int width,int n,double a, double b, double &min_y,double &max_y,double (*func)(double,int, double*, double*, double*),double (*f)(double),double *x, double *coeff,double *tmp);
  void change_ab ();
  double max_f();
  void do_p (double *y,double maxf);
  void realloc ();
  void eval ();

public slots:
  void change_func ();
  void set_func ();
  void change_mode ();
  void set_mode();
  void resize_mult ();
  void resize_dev ();
  void rescale_mult ();
  void rescale_dev ();
  void add_p ();
  void sub_p ();
  
  //void resize ();

protected:
  void paintEvent (QPaintEvent *event);
};



#endif
