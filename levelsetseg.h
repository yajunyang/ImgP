#ifndef LEVELSETSEG_H
#define LEVELSETSEG_H

#include <QString>
#include<cv.h>
#include<highgui.h>
#include<iostream>
#include<cmath>
#include<ctime>
#include <QImage>
#include "imageview.h"

class LevelSetSeg
{
public:
  LevelSetSeg(ImageView *p);
  void run();
  void run(const QString &);

  /**
   * @brief run
   * The parameter must be an image with 1-bit depth
   * @param img
   */
  void run(IplImage *img);

  void ImgDraw(IplImage* src,IplImage *u,IplImage*dst);
  void ImgDraw2(IplImage* src,IplImage *u,IplImage*dst);
  void NeumannBoundCond(IplImage * img);
  void Sobel(IplImage *src, IplImage *dx, IplImage *dy);
  void Evolution2(IplImage * u, IplImage *g, double lambda, double mu, double alf, double epsilon, double delt, int numIter);
  void Evolution(IplImage *u, IplImage *g, double lambda, double mu, double alf, double epsilon, double delt, int numIter);
  void CurvatureCentral(IplImage *nx, IplImage *ny, IplImage* dst);
  void CurvatureCentral2(IplImage *nx, IplImage *ny, IplImage* dst);
  void Dirac(IplImage *src, IplImage *dst, double sigma);

  void initialImage(IplImage *img);

private:

  ImageView* pWindow;
  IplImage *currentImage;
};

#endif // LEVELSETSEG_H
