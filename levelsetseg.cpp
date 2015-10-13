#include "levelsetseg.h"

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <cstdio>
#include <string>
#include "Convert.h"

LevelSetSeg::LevelSetSeg(ImageView* p)
{
  if(p==NULL) return;
  pWindow = p;
  currentImage = 0;
}

void LevelSetSeg::initialImage(IplImage *img)
{
  this->currentImage = cvCloneImage(img);
}

void LevelSetSeg::run()
{
  Q_ASSERT(currentImage != 0);
  Q_ASSERT(currentImage->depth != 1);
  run(currentImage);
}

void LevelSetSeg::run(IplImage *image)
{
  if(image == NULL)
    return;
  CvSize size=cvGetSize(image);
  int i,j;
  IplImage * image_gray_a=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
  IplImage * display1=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
  IplImage * display2=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
  IplImage * display3=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
  IplImage * display_contour=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,3);
  IplImage * imgDisplay=cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,3);

  IplImage * image32=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * imgGauss=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * image_laplace=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * imgG=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * imgU=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * imgU_temp=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * Ix=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * Iy=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);
  IplImage * Edge=cvCreateImage(cvGetSize(image),IPL_DEPTH_32F,1);

  int iterNum = 850;

  cvCvtColor(image,image_gray_a,CV_BGR2GRAY);
  cvConvertScale(image_gray_a,image32,1,0);
  cvSmooth(image32,imgGauss,CV_GAUSSIAN,0,0,1.5,0);

  Sobel(imgGauss,Ix,Iy);

  CvScalar cur,cur1,cur2;
  for (i=0; i<size.height; i++)
    {
      for(j=0; j<size.width; j++)
        {
          cur1 = cvGet2D(Ix,i,j);
          cur2 = cvGet2D(Iy,i,j);
          cur.val[0] = 1.0/(1.0+cur1.val[0]*cur1.val[0]+cur2.val[0]*cur2.val[0]);
          cvSet2D(imgG,i,j,cur);
        }
    }

  int w=15;
  int w2=0;
  double c0=14.0;
  //define initial level set function (LSF) as -c0, 0, c0 at points outside, on
  //the boundary, and inside of a region R, respectively.
  for (i=0; i<size.height; i++)
    {
      for(j=0; j<size.width; j++)
        {
          if (i<w || i>size.height-w-1 || j<w || j>size.width-w-1)
            {
              cur.val[0] = c0;
            }
          else if (i>w && i<size.height-w-1 && j>w && j<size.width-w-1)
            {
              cur.val[0] = -c0;
            }
          else cur.val[0] = 0;
          // Note: this can be commented out. The intial LSF does NOT necessarily need a zero level set.
          cvSet2D(imgU,i,j,cur);
        }
    }

  double epsilon=1.5;//1.5
  double timestep=7;//7
  double lambda=10;//5
  double mu=0.2/timestep;
  double alf=10.5;//1.5

  for (int k=0;k<iterNum;k++)
    {
      Evolution2(imgU,imgG,lambda,mu,alf,epsilon,timestep,1);
      if (k%20==0)
        {
          ImgDraw(image, imgU, imgDisplay);
#if(OPENCV_SHOW_MODE || 1)
          cvShowImage("LevelSet",imgDisplay);
#else
          showInImageView(imgDisplay);
#endif
          /// @warning
          char c =  cvWaitKey(40);  // It's very bad here
          if(c==27) break;
        }
    }

  cvDestroyAllWindows();
  cvReleaseImage(&image_gray_a);
  cvReleaseImage(&display1);
  cvReleaseImage(&display2);
  cvReleaseImage(&display3);
  cvReleaseImage(&display_contour);
  cvReleaseImage(&imgDisplay);
  cvReleaseImage(&Ix);
  cvReleaseImage(&Iy);
  cvReleaseImage(&imgG);
  cvReleaseImage(&imgU);
  cvReleaseImage(&imgU_temp);
  cvReleaseImage(&imgGauss);
  cvReleaseImage(&image32);
  cvReleaseImage(&image_laplace);
  cvReleaseImage(&image);
}

void LevelSetSeg::run(const QString &file)
{
  IplImage * image = cvLoadImage(file.toStdString().c_str(), 1);
  this->run(image);
}

void LevelSetSeg::ImgDraw(IplImage *src, IplImage *u, IplImage *dst)
{
  if (!src || !dst || !u)
    {
      return;
    }

  int i,j;
  CvScalar Csrc, Cu[2], Cred;
  Cred.val[0] = Cred.val[1] = 0;
  Cred.val[2] = 255;

  for (i=0; i<src->height; i++)
    {
      for(j=0; j<src->width-1; j++)
        {
          Cu[0] = cvGet2D(u,i,j);
          Cu[1] = cvGet2D(u,i,j+1);
          Csrc = cvGet2D(src,i,j);
          if (Cu[0].val[0]*Cu[1].val[0]<=0)
            {
              if (fabs(Cu[0].val[0])<fabs(Cu[1].val[0]))
                {
                  cvSet2D(dst,i,j,Cred);
                  continue;
                }
              else
                {
                  cvSet2D(dst,i,j,Csrc);
                  cvSet2D(dst,i,j+1,Cred);
                  j++;
                  continue;
                }
            }
          cvSet2D(dst,i,j,Csrc);
        }
    }

  for (j=0; j<src->width; j++)
    {
      for(i=0; i<src->height-1; i++)
        {
          Cu[0] = cvGet2D(u,i,j);
          Cu[1] = cvGet2D(u,i+1,j);
          Csrc = cvGet2D(src,i,j);
          if (Cu[0].val[0]*Cu[1].val[0]<=0)
            {
              if (fabs(Cu[0].val[0])<fabs(Cu[1].val[0]))
                {
                  cvSet2D(dst,i,j,Cred);
                  continue;
                }
              else
                {
                  cvSet2D(dst,i,j,Csrc);
                  cvSet2D(dst,i+1,j,Cred);
                  i++;
                  continue;
                }
            }
          cvSet2D(dst,i,j,Csrc);
        }
    }
}

void LevelSetSeg::ImgDraw2(IplImage* src,IplImage *u,IplImage*dst)
{
  if(!src||!dst||!u)
    {
      return;
    }
  int i,j;
  CvScalar Csrc,Cu,Cred;
  Cred.val[0]=Cred.val[1]=0;
  Cred.val[2]=255;
  for(i=0;i<src->height;i++)
    {
      for(j=0;j<src->width;j++)
        {
          Cu=cvGet2D(u,i,j);
          Csrc=cvGet2D(src,i,j);
          if(Cu.val[0]==0)
            {
              cvSet2D(dst,i,j,Cred);
            }
          else
            {
              cvSet2D(dst,i,j,Csrc);
            }
        }
    }
}

void LevelSetSeg::NeumannBoundCond(IplImage *img)
{
  if (!img) return;

  int row = img->height;
  int col = img->width;
  int i,j;

  cvSet2D(img, 0, 0, cvGet2D(img, 2, 2));
  cvSet2D(img, 0, col-1, cvGet2D(img, 2, col-3));
  cvSet2D(img, row-1, 0, cvGet2D(img, row-3, 2));
  cvSet2D(img, row-1, col-1, cvGet2D(img, row-3, col-3));

  for (j=1;j<col-1;j++)
    {
      cvSet2D(img, 0, j, cvGet2D(img, 2, j));
      cvSet2D(img, row-1, j, cvGet2D(img, row-3, j));
    }

  for (i=1;i<row-1;i++)
    {
      cvSet2D(img, i, 0, cvGet2D(img, i, 2));
      cvSet2D(img, i, col-1, cvGet2D(img, i, col-3));
    }
}

void LevelSetSeg::Sobel(IplImage *src, IplImage *dx, IplImage *dy)
{
  if (!src || !dx || !dy)
    {
      return;
    }

  cvSobel(src, dx, 1, 0, 1);
  cvSobel(src, dy, 0, 1, 1);

  CvScalar cur;
  for (int i=0;i<src->height;i++)
    {
      for (int j=0;j<src->width;j++)
        {
          cur = cvGet2D(dx,i,j);
          cur.val[0] /= 2.0;
          cvSet2D(dx,i,j,cur);

          cur = cvGet2D(dy,i,j);
          cur.val[0] /= 2.0;
          cvSet2D(dy,i,j,cur);
        }
    }
}

void LevelSetSeg::Evolution2(IplImage *u, IplImage *g, double lambda, double mu, double alf, double epsilon, double delt, int numIter)
{
  if (!u||!g)
    return;
  CvSize size = cvGetSize(u);
  IplImage* vx = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* vy = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* ux = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* uy = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* Nx = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* Ny = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* diracU = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* K = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* Laplace = cvCreateImage(size,IPL_DEPTH_32F,1);

  Sobel(g,vx,vy);
  CvScalar s1,s2,s11,s22;
  CvScalar Cdirac, Cvx, Cvy, CNx, CNy, Cg, CK, CLaplace, Cu;
  int i,j;
  for(int k=0;k<numIter;k++)
    {
      std::cout<<k<<"……"<<std::endl;
      NeumannBoundCond(u);
      Sobel(u,ux,uy);

      for(i=0;i<size.height;i++)
        {
          for(j=0;j<size.width;j++)
            {
              s1=cvGet2D(ux,i,j);
              s2=cvGet2D(uy,i,j);
              double normDu=sqrt(pow(s1.val[0],2)+pow(s2.val[0],2)+1e-10);
              s11.val[0]=s1.val[0]/normDu;
              s22.val[0]=s2.val[0]/normDu;
              cvSet2D(Nx,i,j,s11);
              cvSet2D(Ny,i,j,s22);
            }
        }

      Dirac(u,diracU,epsilon);
      CurvatureCentral2(Nx,Ny,K);
      cvLaplace(u, Laplace, 1);
      for(i=0;i<size.height;i++)
        {
          for(j=0;j<size.width;j++)
            {
              Cdirac=cvGet2D(diracU,i,j);
              Cvx=cvGet2D(vx,i,j);
              Cvy=cvGet2D(vy,i,j);
              CNx=cvGet2D(Nx,i,j);
              CNy=cvGet2D(Ny,i,j);
              Cg=cvGet2D(g,i,j);
              CK=cvGet2D(K,i,j);
              CLaplace=cvGet2D(Laplace,i,j);
              Cu=cvGet2D(u,i,j);

              double weightedLengthTerm=lambda*Cdirac.val[0]*(Cvx.val[0]*CNx.val[0]+Cvy.val[0]*CNy.val[0]+Cg.val[0]*CK.val[0]);
              double weightedAreaTerm=alf*Cdirac.val[0]*Cg.val[0];
              double penalizingTerm=mu*(CLaplace.val[0]-CK.val[0]);
              double total=weightedLengthTerm+weightedAreaTerm+penalizingTerm;
              Cu.val[0]+=delt*total;
              cvSet2D(u,i,j,Cu);
            }
        }
    }
  cvReleaseImage(&vx);
  cvReleaseImage(&vy);
  cvReleaseImage(&ux);
  cvReleaseImage(&uy);
  cvReleaseImage(&Nx);
  cvReleaseImage(&Ny);
  cvReleaseImage(&diracU);
  cvReleaseImage(&K);
  cvReleaseImage(&Laplace);
}

void LevelSetSeg::CurvatureCentral(IplImage *nx, IplImage *ny, IplImage* dst)
{
  if (!nx || !ny || !dst)
    {
      return;
    }

  CvSize size = cvSize(nx->width, nx->height);
  IplImage* nxx = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* nyy = cvCreateImage(size,IPL_DEPTH_32F,1);

  Sobel(nx, nxx, nyy);

  CvScalar cx,cy,crt;
  for (int i=0; i<size.height; i++)
    {
      for (int j=0; j<size.width; j++)
        {
          cx = cvGet2D(nxx,i,j);
          cy = cvGet2D(nyy,i,j);
          crt.val[0] = cx.val[0] + cy.val[0];
          cvSet2D(dst,i,j,crt);
        }
    }

  cvReleaseImage(&nxx);
  cvReleaseImage(&nyy);
}

void LevelSetSeg::CurvatureCentral2(IplImage *nx, IplImage *ny, IplImage* dst)
{
  if (!nx || !ny || !dst)
    {
      return;
    }

  CvSize size = cvSize(nx->width, nx->height);
  IplImage* nxx = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* nyy = cvCreateImage(size,IPL_DEPTH_32F,1);
  IplImage* junk = cvCreateImage(size,IPL_DEPTH_32F,1);

  Sobel(nx,nxx,junk);
  Sobel(ny,junk,nyy);

  CvScalar cx,cy,crt;
  for (int i=0; i<size.height; i++)
    {
      for (int j=0; j<size.width; j++)
        {
          cx = cvGet2D(nxx,i,j);
          cy = cvGet2D(nyy,i,j);
          crt.val[0] = cx.val[0] + cy.val[0];
          cvSet2D(dst,i,j,crt);
        }
    }

  cvReleaseImage(&nxx);
  cvReleaseImage(&nyy);
  cvReleaseImage(&junk);
}

void LevelSetSeg::Dirac(IplImage *src, IplImage *dst, double sigma)
{
  if (!src || !dst)
    {
      return;
    }

  int i,j;
  CvScalar Csrc, Cdst;
  double tmp = 1.0/2.0/sigma;

  for (i=0; i<src->height; i++)
    {
      for(j=0; j<src->width; j++)
        {
          Csrc = cvGet2D(src,i,j);
          if (Csrc.val[0]<=sigma && Csrc.val[0]>=-sigma)
            {
              Cdst.val[0] = tmp*(1+cos(CV_PI*Csrc.val[0]/sigma));
            }
          else
            {
              Cdst.val[0] = 0;
            }
          cvSet2D(dst,i,j,Cdst);
        }
    }
}
