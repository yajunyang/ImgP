#include <QApplication>
#include "imageview.h"
#include <iostream>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ImageView w;
  w.show();
  a.exec();
}
