#ifndef CONVERT_H
#define CONVERT_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cv.h>
#include <highgui.h>
#include <core/core.hpp>
#include <QPixmap>
#include <qimage.h>
#include <iostream>
#include <cstring>
#include <QString>

#define MAX_ALLOWED_SIZE 600
#define OPENCV_SHOW_MODE 0

inline float random()
{
  int N = 666;
  srand(time(NULL));
  float r = rand()%(N+1)/(float)(N+1);
  return r;
}

typedef unsigned short uint16_t;

using std::string;
using std::iostream;


/**
 * @brief ImageToIplImage    Remember to release the iplimage
 * @param qPix
 * @return
 */
IplImage * ImageToIplImage(QPixmap *);


IplImage *toGray(IplImage *);

QImage *IplImageToQImage(const IplImage * , uchar **, double , double );

QImage *IplImage2QImageLowEffect(IplImage *);

#endif // CONVERT_H
