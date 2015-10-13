#ifndef GLSM_H
#define GLSM_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "ui_GLSM.h"
#include "imageview.h"
#include <QDialog>

using namespace std;
typedef unsigned char byte;

class GLCM : public QDialog
{
  Q_OBJECT

public:
  typedef struct glcmFeature
  {
    double    dCorrelation;  //相关
    double    dEnergy;       //能量
    double    dEntropy;       //熵
    double    dInertiaQuadrature; //对比度
    double    dLocalCalm;         //逆差距
  }glcmFeature;

  typedef struct glcmFeatureVar
  {
    double    dAveCorrelation;
    double    dAveEnergy;
    double    dAveEntropy;
    double    dAveInertiaQuadrature;
    double    dAveLocalCalm;   //四个方向的共现矩阵特征的均值 Ave
    double    dVarCorrelation;
    double    dVarEnergy;
    double    dVarEntropy;
    double    dVarInertiaQuadrature;
    double    dVarLocalCalm;   //四个方向的共现矩阵特征的方差 Var
  }glcmFeatureVar;

private:
  glcmFeature pGLCMF;
  glcmFeatureVar pGLCMFVar;
  int FilterWindowWidth;
  int distance;
  int GrayLayerNum;
  int L;

  //! 共现矩阵
  int** PMatrixRD; //45度方向上的灰度共现矩阵
  int** PMatrixLD; //135度方向上的灰度共现矩阵
  int** PMatrixV;  //90度方向上的灰度共现矩阵
  int** PMatrixH;  //0度方向上的灰度共现矩阵

  Ui::Form *ui;

public:
  explicit GLCM(QWidget *parent = 0);
  ~GLCM();

  void ComputeMatrix(byte **LocalImage, int LocalImageWidth);
  void ComputeFeature(double &FeatureEnergy, double &FeatureEntropy,
                      double &FeatureInertiaQuadrature,
                      double &FeatureCorrelation, double &FeatureLocalCalm, int** pMatrix, int dim);
  glcmFeature GLCMFeature(byte* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth,int dir);
  glcmFeatureVar GLCMFeatureVar(byte* ImageArray,long ImageWidth,long ImageHeight,int FilterWindowWidth);


  //!
  //! \brief runGLCMAndGetParams
  //!
  void runGLCMAndGetParams(IplImage *);


  double FeatureLocalCalmRD;
  double FeatureLocalCalmLD;
  double FeatureLocalCalmV;
  double FeatureLocalCalmH;
  double FeatureCorrelationRD;
  double FeatureCorrelationLD;
  double FeatureCorrelationV;
  double FeatureCorrelationH;
  double FeatureInertiaQuadratureRD;
  double FeatureInertiaQuadratureLD;
  double FeatureInertiaQuadratureV;
  double FeatureInertiaQuadratureH;
  double FeatureEntropyRD;
  double FeatureEntropyLD;
  double FeatureEntropyV;
  double FeatureEntropyH;
  double FeatureEnergyRD;
  double FeatureEnergyLD;
  double FeatureEnergyV;
  double FeatureEnergyH;
  //!

public slots:
  void saveData();
};


#endif // GLSM_H
