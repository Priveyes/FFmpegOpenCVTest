#include "opencv/cv.h"
#include <opencv2/core/core.hpp>        // Basic OpenCV structures 
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class CFishEyeCorrect
{
public:
	int dstWidth;
	int dstHeight;
	double cx;
	double cy;
	double f;
	double r;
	double phi0;
	double sita0;
	double gama0;
	double phi1;
	double sita1;
	double gama1;
	double phi2;
	double sita2;
	double gama2;
	double phi3;
	double sita3;
	double gama3;
	double wRatio;
	double hRatio;
    double dux;
    double dvx;
    double duy;
    double dvy;
    double duz;
    double dvz;
	int * position;
	long * positionShift;
	Mat orgImg;
	Mat dstImg;
	CFishEyeCorrect();
	~CFishEyeCorrect();
	Rect correctArea;
	Rect GetArea(const Mat &inputImage);
	void ParamFix();
	void FixPosition(const Mat& orgImg);
	void CalculateParam(int width, int height);
	void Fix(int &u,int &v,double &x,double &y,const double &phi,const double &sita,const double &gama);
	void CalcPositon();
	void NewImageCorrect(const Mat& orgImg,Mat& dstImg);

    Mat angleDewarpMap_x, angleDewarpMap_y;

    Mat unfoldDstImg;
    int dstRow;
    int dstCol;
    int * unfoldPosition;
	void calcUnfoldPosition();
	void unfold(const Mat& orgImg, Mat& unfoldDstImg);
	Mat unfoldMap_x, unfoldMap_y;
private:
	const double piParam;
	void FindCorrectArea(const Mat &inputImage);
};
