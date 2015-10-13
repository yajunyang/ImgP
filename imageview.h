#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QMainWindow>
#include <QtCore>
#include <QVector>

class QTimer;
class QPushButton;
class GLCM;
class DialogCamera;

namespace Ui {
  class ImageView;
}

class ImageView : public QMainWindow
{
  Q_OBJECT

public:
  explicit ImageView(QWidget *parent = 0);

  void updateImage(const QPixmap&);
  void update();

  ~ImageView();

private slots:
  void openDir();
  void openFile();
  void save();
  void saveAs();

  void nextImage();
  void previousImage();
  void setBegin();
  void setEnd();
  void slideShow();
  void setSlideShowDelay(int delay);

  //! Image Processing Action
  //!
  void median_smoooth();
  void gaussian_smooth();
  void bilateral_smooth();
  void gLCM();
  void canny();
  void levelSetSegmentation();
  void videoCameraTrack();

protected:
  void resizeEvent(QResizeEvent *event);
  void wheelEvent(QWheelEvent *);

private:
  //! \brief Notice here
  //!
  GLCM* glcm;
  DialogCamera* diaCam;

  Ui::ImageView *ui;
  QString folderName;
  QFileInfoList fileList;


  //! // Notice Here
  //!
  QPixmap currentPixmap;

  int imageNumber;
  int totalImages;
  float slideShowDelay;
  QTimer* slideshowTimer;

  void updateGUI();
  void setImage(QString filePath);
  void setButtonIcon(QPushButton* button, QString fileName, int width, int size);


  //! Connect Image Processing Actions to Slots Methods
  //!
  void connectActionToSlots();
  void newSonWidgets();
};

#endif // IMAGEVIEW_H
