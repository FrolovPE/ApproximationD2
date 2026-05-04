#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>

class Window : public QWidget
{
  Q_OBJECT

private:
  int func_id;
  const char *f_name;
  double a, b, c, d;
  double a0, b0, c0, d0;
  int nx, ny;
  int mx, my;
  double eps;
  int mi;
  int p;

  int k;
  double (*f) (double, double);
  int draw_mode;
  const char *mode_name;
  int scale;
  int P;

  double *A;
  double *B;
  int    *I;
  double *coeff;
  double *r, *u, *v, *sp;

  double max_f ();
  void   realloc ();
  void   change_ab ();
  void   set_func ();
  void   set_mode ();

public:
  Window (QWidget *parent);
  ~Window ();

  QSize minimumSizeHint () const;
  QSize sizeHint () const;

  int parse_command_line (int argc, char *argv[]);
  void eval ();

public slots:
  void change_func ();
  void change_mode ();
  void resize_mult ();
  void resize_dev ();
  void rescale_mult ();
  void rescale_dev ();
  void add_p ();
  void sub_p ();
  void resize_viz_mult ();
  void resize_viz_dev ();

protected:
  void paintEvent (QPaintEvent *event);
};

#endif
