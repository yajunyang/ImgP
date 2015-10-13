#ifndef CAPTURE_H
#define CAPTURE_H
#include<cv.h>
#include<highgui.h>
#include <QString>

class Capture
{
public:
  Capture();
  void run();

  void setFileName(const QString &);

  void setFormat(bool isCamera)
  {
    this->isCamera = isCamera;
  }

  void showFrame(CvCapture *);

private:
  QString fileName;
  bool isCamera;
};

#endif // CAPTURE_H
