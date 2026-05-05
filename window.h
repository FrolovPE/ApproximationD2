#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>
#include "args.h"

class args;

class Window : public QWidget
{
  Q_OBJECT

private:

  //sostoyania
  static const int BUSY = 0;
  static const int READY = 1; // mojno vipolnyat' comandi
  static const int DONE = 2;  // dealem swap coeffs

  int ready;

  QTimer timer;

  pthread_t *tid;
  args *ap;
  bool has_res = false;
  bool close_requested = false;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  int it;

  char *argv0;
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
  double *coeff_work;
  double *r, *u, *v, *sp;
  double *vals;
  int     vals_size;

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
  bool can_recompute() ;
  int get_ready();
  void set_ready(int value);

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
  void check_recompute();
  void check_n_close();

protected:
  void paintEvent (QPaintEvent *event);
  void closeEvent (QCloseEvent *event);
};

#endif
