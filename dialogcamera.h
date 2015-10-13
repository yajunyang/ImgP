#ifndef DIALOGCAMERA_H
#define DIALOGCAMERA_H

#include <QDialog>

namespace Ui {
  class DialogCamera;
}

class DialogCamera : public QDialog
{
  Q_OBJECT

public:
  explicit DialogCamera(QWidget *parent = 0);
  ~DialogCamera();

private slots:
  void cameraOpen();
  void videoOpen();

private:
  Ui::DialogCamera *ui;
};

#endif // DIALOGCAMERA_H
