#include "capture.h"

Capture::Capture()
{
  isCamera = false;
}

void Capture::run()
{
  if(isCamera)
    {
      CvCapture *pCapture = pCapture = cvCaptureFromCAM(0);//Read camera
      if(NULL == pCapture)
        return;
      showFrame(pCapture);
      cvReleaseCapture(&pCapture);
    }else{
      if(!fileName.isEmpty())
        {
          CvCapture *pCapture = cvCreateFileCapture(fileName.toStdString().c_str());  // Read video
          if(NULL == pCapture)
            return;
          showFrame(pCapture);
          cvReleaseCapture(&pCapture);
        }
    }
}

void Capture::showFrame(CvCapture *pCapture)
{
  if(pCapture == NULL)
    return;

  IplImage* pFrame = NULL;
  IplImage* pFrImg = NULL;
  IplImage* pBkImg = NULL;

  CvMat* pFrameMat = NULL;
  CvMat* pFrMat = NULL;
  CvMat* pBkMat = NULL;

  CvVideoWriter * writer=0;

  int nFrmNum = 0;
  int fps=100;

  cvNamedWindow("video", 1);
  cvNamedWindow("background",1);
  cvNamedWindow("foreground",1);

  cvMoveWindow("video", 30, 0);
  cvMoveWindow("background", 360, 0);
  cvMoveWindow("foreground", 690, 0);


  while(pFrame = cvQueryFrame( pCapture ))
    {
      cvWriteFrame(writer,pFrame);
      nFrmNum++;

      if(nFrmNum == 1)
        {
          pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);
          pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);

          pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
          pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
          pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

          cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
          cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

          cvConvert(pFrImg, pFrameMat);
          cvConvert(pFrImg, pFrMat);
          cvConvert(pFrImg, pBkMat);
        }
      else
        {
          cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
          cvConvert(pFrImg, pFrameMat);

          cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 0, 0, 0.8);
          IplImage* img1 = cvCreateImage(cvSize(pFrame->width, pFrame->height),  IPL_DEPTH_8U,1);

          cvAbsDiff(pFrameMat, pBkMat, pFrMat);
          cvConvert(pFrMat,img1);

          cvThreshold(pFrMat, pFrImg, 40, 255.0, CV_THRESH_BINARY);
          cvSmooth(pFrImg,pFrImg,CV_MEDIAN,5,5);

          //cvErode(pFrImg, pFrImg, 0, 1);
          //cvDilate(pFrImg, pFrImg, 0, 1);

          cvRunningAvg(pFrameMat, pBkMat, 0.3, 0);
          //cvAddWeighted(pFrameMat, 0.3,pBkMat,0.7,0,pBkMat);

          cvConvert(pBkMat, pBkImg);

          cvShowImage("video", pFrame);
          cvShowImage("background", pBkImg);
          cvShowImage("foreground", pFrImg);

          if( cvWaitKey(10000/fps) >= 0 )
            break;
        }
    }

  cvDestroyWindow("video");
  cvDestroyWindow("background");
  cvDestroyWindow("foreground");
  cvReleaseImage(&pFrImg);
  cvReleaseImage(&pBkImg);
  cvReleaseMat(&pFrameMat);
  cvReleaseMat(&pFrMat);
  cvReleaseMat(&pBkMat);
  cvReleaseVideoWriter(&writer);
}

void Capture::setFileName(const QString &fileName)
{
  this->fileName = fileName;
}
