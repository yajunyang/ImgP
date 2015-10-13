#include <QFileDialog>
#include <QMessageBox>
#include <QWheelEvent>
#include <QDir>
#include "Convert.h"
#include "imageview.h"
#include "ui_imageview.h"
#include "glcm.h"
#include "dialogcamera.h"
#include "levelsetseg.h"

ImageView::ImageView(QWidget *parent) : QMainWindow(parent), ui(new Ui::ImageView)
{
  ui->setupUi(this);

  setWindowTitle("MicroLab ImgP");

  connectActionToSlots();
  newSonWidgets();

  connect(ui->openDirAction, SIGNAL(triggered()), this, SLOT(openDir()));
  connect(ui->openFileAction, SIGNAL(triggered()), this, SLOT(openFile()));
  connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
  connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveAs()));
  connect(ui->actionExit_4, SIGNAL(triggered()), this, SLOT(close()));

  connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(nextImage()));
  connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(previousImage()));
  connect(ui->beginButton, SIGNAL(clicked()), this, SLOT(setBegin()));
  connect(ui->endButton, SIGNAL(clicked()), this, SLOT(setEnd()));
  connect(ui->slideShowButton, SIGNAL(clicked()), this, SLOT(slideShow()));
  connect(ui->slideShowDelaySlider, SIGNAL(valueChanged(int)), this, SLOT(setSlideShowDelay(int)));
  slideshowTimer = new QTimer(this);
  connect(slideshowTimer, SIGNAL(timeout()), this, SLOT(nextImage()));

  ui->imageNameLabel->setText(tr("Select a folder from the <b>file -> openDir</b> or <b>file->openFile</b> menu to view images or image.."));
  ui->openFileAction->setShortcut(tr("Ctrl+O"));
  setImage(":/images/Cover.jpg");

  ui->previousButton->setEnabled(false);
  ui->nextButton->setEnabled(false);
  ui->beginButton->setEnabled(false);
  ui->endButton->setEnabled(false);
  ui->slideShowButton->setEnabled(false);
  ui->slideShowDelaySlider->setEnabled(false);
  ui->slideShowDelayLabel->setEnabled(false);
  ui->slideShowDelaySlider->setValue(15);
  ui->actionSave->setEnabled(false);

  ui->actionLecel_Set->setEnabled(false); //! @warning  Bug

  setButtonIcon(ui->slideShowButton, ":/images/buttons/play.png", 48, 48);
  setButtonIcon(ui->nextButton, ":/images/buttons/next.png", 48, 48);
  setButtonIcon(ui->previousButton, ":/images/buttons/back.png", 48, 48);
  setButtonIcon(ui->beginButton, ":/images/buttons/rewind.png", 48, 48);
  setButtonIcon(ui->endButton, ":/images/buttons/fastfwd.png", 48, 48);

  setWindowTitle(tr("ImageView"));

  folderName = "";
  imageNumber = 0;
  totalImages = 0;
  slideShowDelay = 2.0;

  ui->statusBar->showMessage("If any question, please connect me at http://yajunyang.cn");
}

ImageView::~ImageView()
{
  delete ui;
}

void ImageView::setButtonIcon(QPushButton *button, QString fileName, int width, int height)
{
  QPixmap iconPixmap(fileName);
  QIcon buttonIcon(iconPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  button->setIcon(buttonIcon);
  button->setIconSize(iconPixmap.scaled(width, height).rect().size());
}


//! SLOTS
//!
//!
void ImageView::openDir()
{
  folderName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "./", QFileDialog::ShowDirsOnly);

  if (folderName == "") {
      return;
    }

  QDir imageDir(folderName);
  QStringList imageFilters;
  imageFilters << "*.bmp" << "*.png" << "*.jpg" << "*.jpeg" << "*.gif";

  imageDir.setNameFilters(imageFilters);
  imageDir.setFilter(QDir::Files);
  imageDir.setSorting(QDir::Name);

  fileList.clear();
  fileList = imageDir.entryInfoList();

  imageNumber = 0;
  totalImages = fileList.size();

  updateGUI();
  if(!ui->actionSave->isEnabled())
    ui->actionSave->setEnabled(true);
}

void ImageView::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty()) {
      totalImages = 1;
      ui->slideShowButton->setEnabled(false);
      ui->slideShowDelayLabel->setEnabled(false);
      ui->slideShowDelaySlider->setEnabled(false);
      setImage(fileName);
      ui->imageNameLabel->setText(fileName);
      if(!ui->actionSave->isEnabled())
        ui->actionSave->setEnabled(true);
    }
}

void ImageView::save()
{
  //    currentPixmap.toImage().save(ui->imageNameLabel->text(), "JPEG");
  qDebug(ui->imageNameLabel->text().toStdString().c_str());
}

void ImageView::saveAs()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                  "/home/jana/untitled.png",
                                                  tr("Images (*.png *.xpm *.jpg)"));
  if(!fileName.isEmpty())
    currentPixmap.toImage().save(fileName, 0, 100);
}

void ImageView::previousImage()
{
  imageNumber--;
  updateGUI();
}

void ImageView::nextImage()
{
  imageNumber++;
  if (imageNumber > totalImages-1) {
      imageNumber = 0;
    }
  updateGUI();
  if(slideshowTimer->isActive()) {
      slideshowTimer->start((int)(slideShowDelay * 1000));
    }
}

void ImageView::setBegin()
{
  if (totalImages == 0) {
      return;
    }
  imageNumber = 0;
  updateGUI();
}

void ImageView::setEnd()
{
  if (totalImages == 0) {
      return;
    }
  imageNumber = totalImages-1;
  updateGUI();
}

void ImageView::updateGUI()
{
  if (totalImages == 0 ) {
      ui->nextButton->setEnabled(false);
      ui->previousButton->setEnabled(false);
      ui->beginButton->setEnabled(false);
      ui->endButton->setEnabled(false);
      ui->slideShowButton->setEnabled(false);
      ui->slideShowDelayLabel->setEnabled(false);
      ui->slideShowDelaySlider->setEnabled(false);
      ui->imageLabel->clear();
      ui->imageNameLabel->setText(tr("No image files found.\nSelect a folder with image files (*.png, *.jpg, *.gif, *.bmp)."));
      ui->imageNumberLabel->setText("0 / 0");

      return;
    }

  if (totalImages == 1) {
      ui->slideShowButton->setEnabled(false);
      ui->slideShowDelayLabel->setEnabled(false);
      ui->slideShowDelaySlider->setEnabled(false);

    } else {
      ui->slideShowButton->setEnabled(true);
      ui->slideShowDelayLabel->setEnabled(true);
      ui->slideShowDelaySlider->setEnabled(true);
    }

  QFileInfo currentFile = fileList.at(imageNumber);
  setImage(currentFile.absoluteFilePath());

  QString number = QString::number(imageNumber+1) + " / " + QString::number(totalImages);
  ui->imageNumberLabel->setText(number);

  ui->imageNameLabel->setText(currentFile.absoluteFilePath());

  ui->previousButton->setEnabled(true);
  ui->nextButton->setEnabled(true);
  ui->beginButton->setEnabled(true);
  ui->endButton->setEnabled(true);

  if (imageNumber == 0) {
      ui->previousButton->setEnabled(false);
    }
  if (imageNumber == totalImages-1) {
      ui->nextButton->setEnabled(false);
    }
}

void ImageView::resizeEvent(QResizeEvent* /* event */)
{
  if (!currentPixmap.isNull()) {
      QSize imageSize = currentPixmap.size();
      if (imageSize.width() < ui->imageLabel->width() && imageSize.height() < ui->imageLabel->height()) {
          ui->imageLabel->setPixmap(currentPixmap);
        } else {
          ui->imageLabel->setPixmap(currentPixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void ImageView::wheelEvent(QWheelEvent *)
{

}


//!
//! \brief ImageView::setImage
//! \param filePath
//!
void ImageView::setImage(QString filePath)
{
  currentPixmap = QPixmap(filePath);

  if (currentPixmap.isNull()) {
      QMessageBox::information(this, tr("File open error..."), tr("Cannot open file \"%1\"").arg(filePath));
      return;
    }

  QSize imageSize = currentPixmap.size();
  if (imageSize.width() < ui->imageLabel->width() && imageSize.height() < ui->imageLabel->height()) {
      ui->imageLabel->setPixmap(currentPixmap);
    } else {
      ui->imageLabel->setPixmap(currentPixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

}

void ImageView::slideShow()
{
  if (!slideshowTimer->isActive()) {
      slideshowTimer->start((int)(slideShowDelay * 1000));
      setButtonIcon(ui->slideShowButton, ":/images/buttons/pause.png", 48, 48);
    } else {
      slideshowTimer->stop();
      setButtonIcon(ui->slideShowButton, ":/images/buttons/play.png", 48, 48);
    }
}

void ImageView::setSlideShowDelay(int delay)
{
  slideShowDelay = 0.5 + delay * 0.1;
  ui->slideShowDelayLabel->setText(QString::number(slideShowDelay) + " s.");
  if (slideshowTimer->isActive()) {
      slideshowTimer->start((int)(slideShowDelay * 1000));
    }
}



//!
//! \brief ImageView::updateImage
//! \param pixmap
//!
void ImageView::updateImage(const QPixmap &pixmap)
{
  currentPixmap = pixmap;
  if (currentPixmap.isNull()) {
      QMessageBox::warning(this, tr("Image Assigning Warning"), tr("The assigning image is empty..."));
      return;
    }

  QSize imageSize = currentPixmap.size();
  if (imageSize.width() < ui->imageLabel->width() && imageSize.height() < ui->imageLabel->height()) {
      ui->imageLabel->setPixmap(currentPixmap);
    } else {
      ui->imageLabel->setPixmap(currentPixmap.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ImageView::update()
{
  updateImage(currentPixmap);
}



//! Connect Image Processing Actions to Slots Methods
//!
void ImageView::connectActionToSlots()
{
  connect(ui->actionMid_Filter_2, SIGNAL(triggered()), this, SLOT(median_smoooth()));
  connect(ui->actionGaussian, SIGNAL(triggered()), this, SLOT(gaussian_smooth()));
  connect(ui->actionBilateral_Smooth,SIGNAL(triggered()), this, SLOT(bilateral_smooth()));
  connect(ui->actionGLCM, SIGNAL(triggered()), this, SLOT(gLCM()));
  connect(ui->actionCanny, SIGNAL(triggered()), this, SLOT(canny()));
  connect(ui->actionLecel_Set, SIGNAL(triggered()), this, SLOT(levelSetSegmentation()));
  connect(ui->actionVideo_Camera_Track, SIGNAL(triggered()), this, SLOT(videoCameraTrack()));
}

void ImageView::newSonWidgets()
{
  glcm = new GLCM(this);
  glcm->setModal(true);

  diaCam = new DialogCamera(this);
  diaCam->setModal(true);
}

void ImageView::median_smoooth()
{
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  if(currentIpl == 0)
    return;

  IplImage *grayImage = toGray(currentIpl);
  IplImage *dst = cvCloneImage(grayImage);
  cvSmooth(grayImage, dst, CV_MEDIAN, 3, 0, 0, 0);
  cvReleaseImage(&grayImage);

  QImage *image = 0;
  if(dst->width > MAX_ALLOWED_SIZE)
    {
      image = IplImage2QImageLowEffect(dst);
    } else
    {
      image = IplImageToQImage(dst, (uchar**)&dst->imageData, 0, 0);
    }
  currentPixmap = QPixmap::fromImage(*image);
  update();   // Update the curreent image

  delete image;
  cvReleaseImage(&dst);
  cvReleaseImage(&currentIpl);
  ui->statusBar->showMessage("Median Smooth");

#if(0) // Testing Code
  IplImage *ipl = cvLoadImage("E:/yang.jpg");
  cvNamedWindow("AAA",CV_WINDOW_AUTOSIZE);
  cvShowImage("AAA", ipl);

  QImage *image = IplImageToQImage(ipl, (uchar**)&ipl->imageData, 0, 0);
  currentPixmap = QPixmap::fromImage(*image);
  update();
  cvWaitKey(0);
  cvReleaseImage(&ipl);
  cvDestroyWindow("AAA");
  delete image;
#endif
}

void ImageView::gaussian_smooth()
{
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  if(currentIpl == 0)
    return;

  IplImage *grayImage = toGray(currentIpl);
  IplImage *dst = cvCloneImage(grayImage);

  cvSmooth(grayImage, dst, CV_GAUSSIAN, 3, 0, 0, 0);

  cvReleaseImage(&grayImage);

  QImage *image = 0;
  if(dst->width > MAX_ALLOWED_SIZE)
    {
      image = IplImage2QImageLowEffect(dst);
    } else
    {
      image = IplImageToQImage(dst, (uchar**)&dst->imageData, 0, 0);
    }
  currentPixmap = QPixmap::fromImage(*image);
  update();   // Update the curreent image

  delete image;
  cvReleaseImage(&dst);
  cvReleaseImage(&currentIpl);
  ui->statusBar->showMessage("Gaussian Smooth");
}

void ImageView::bilateral_smooth()
{
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  if(currentIpl == 0)
    return;

  IplImage *grayImage = toGray(currentIpl);
  IplImage *dst = cvCloneImage(grayImage);
  cvSmooth(grayImage, dst, CV_BILATERAL, 3, 0, 0, 0);
  cvReleaseImage(&grayImage);

  QImage *image = 0;
  if(dst->width > MAX_ALLOWED_SIZE)
    {
      image = IplImage2QImageLowEffect(dst);
    } else
    {
      image = IplImageToQImage(dst, (uchar**)&dst->imageData, 0, 0);
    }
  currentPixmap = QPixmap::fromImage(*image);
  update();   // Update the curreent image

  delete image;
  cvReleaseImage(&dst);
  cvReleaseImage(&currentIpl);
  ui->statusBar->showMessage("Bilateral Smooth");
}

void ImageView::gLCM()
{
  //! If you want use OpenCV to process the current opening "QpixMap" format image.
  //!
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  if(currentIpl == 0)
    return;
  //!

  IplImage *grayImg = toGray(currentIpl);
  if(grayImg ==NULL)
    return;

#if(0)  //! Test color-gray dconvert
  cvNamedWindow("Test Gray", CV_WINDOW_AUTOSIZE);
  cvShowImage("Test Gray", grayImg);
  cvWaitKey(0);
#endif

  //! Deal the new image with GLCM method and show the diagram
  glcm->runGLCMAndGetParams(grayImg);
  if(glcm->isHidden())
    glcm->show();
  //!

  ui->statusBar->showMessage("GLCM Procesing");

  //! Release Memory
  cvReleaseImage(&currentIpl);
  cvReleaseImage(&grayImg);
}

void ImageView::canny()
{
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  if(currentIpl == 0)
    return;
  //!

  IplImage *dst = toGray(currentIpl);

  float r = random();
  int low = static_cast<int>(r*150+0.5);
  int high = static_cast<int>(r*200+0.5);
  cvCanny(dst, dst, low, high);       // ALGORITHMS

  QImage *image = 0;
  if(dst->width > MAX_ALLOWED_SIZE)
    {
      image = IplImage2QImageLowEffect(dst);
    } else
    {
      image = IplImageToQImage(dst, (uchar**)&dst->imageData, 0, 0);
    }
  currentPixmap = QPixmap::fromImage(*image);     /// @warning BIG IMAGE CORRUCTED
  update();   // Update the curreent image

  cvReleaseImage(&dst);
  cvReleaseImage(&currentIpl);
  delete image;
  ui->statusBar->showMessage("Canny");
}

void ImageView::levelSetSegmentation()
{
  IplImage *currentIpl = ImageToIplImage(&currentPixmap);
  Q_ASSERT(currentIpl != 0);

  LevelSetSeg l(this);
  l.initialImage(currentIpl); // deep clone for parameter space
  l.run();

  cvReleaseImage(&currentIpl);
  ui->statusBar->showMessage("Level Set Segmentation");
}

void ImageView::videoCameraTrack()
{
  diaCam->show();
}
