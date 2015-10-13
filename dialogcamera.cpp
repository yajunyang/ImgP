#include "dialogcamera.h"
#include "ui_dialogcamera.h"
#include <QFileDialog>
#include "capture.h"

DialogCamera::DialogCamera(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogCamera)
{
  ui->setupUi(this);
  connect(ui->cameraButton, SIGNAL(clicked()), this, SLOT(cameraOpen()));
  connect(ui->videoButton, SIGNAL(clicked()), this, SLOT(videoOpen()));
}

DialogCamera::~DialogCamera()
{
  delete ui;
}


void DialogCamera::cameraOpen()
{
  Capture capture;
  capture.setFormat(true);
  capture.run();
}

void DialogCamera::videoOpen()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), QDir::currentPath());
  if(fileName.isEmpty())
    return;
  Capture capture;
  capture.setFormat(false);
  capture.setFileName(fileName);
  capture.run();
}
