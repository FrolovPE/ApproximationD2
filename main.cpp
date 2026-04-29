
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>

#include "window.h"

int main (int argc, char *argv[])
{
  QApplication app (argc, argv);

  QMainWindow *window = new QMainWindow;
  QMenuBar *tool_bar = new QMenuBar (window);
  Window *graph_area = new Window (window);
  QAction *action;

  if (graph_area->parse_command_line (argc, argv))
    {
      QMessageBox::warning (0, "Wrong input arguments!", 
                            "Wrong input arguments!");
      return -1;
    }

  action = tool_bar->addAction ("&Change function", graph_area, SLOT (change_func ()));
  action->setShortcut (QString ("0"));

  action = tool_bar->addAction ("&Change mode", graph_area, SLOT (change_mode ()));
  action->setShortcut (QString ("1"));

  action = tool_bar->addAction ("&Rescale x2", graph_area, SLOT (rescale_dev ()));
  action->setShortcut (QString ("2"));

  action = tool_bar->addAction ("&Rescale /2", graph_area, SLOT (rescale_mult ()));
  action->setShortcut (QString ("3"));

  action = tool_bar->addAction ("&Resize n *2", graph_area, SLOT (resize_mult ()));
  action->setShortcut (QString ("4"));

  action = tool_bar->addAction ("&Resize n /2", graph_area, SLOT (resize_dev ()));
  action->setShortcut (QString ("5"));

  action = tool_bar->addAction ("&Add p", graph_area, SLOT (add_p ()));
  action->setShortcut (QString ("6"));

  action = tool_bar->addAction ("&Sub p", graph_area, SLOT (sub_p ()));
  action->setShortcut (QString ("7"));

  action = tool_bar->addAction ("E&xit", window, SLOT (close ()));
  action->setShortcut (QString ("Ctrl+X"));



  tool_bar->setMaximumHeight (30);

  window->setMenuBar (tool_bar);
  window->setCentralWidget (graph_area);
  window->setWindowTitle ("Graph");

  window->show ();
  app.exec ();
  delete window;
  return 0;
}
