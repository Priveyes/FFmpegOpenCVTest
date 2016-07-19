#include "FishEyeCorrect.h"
#include <iostream>

#include <android/log.h>
#define  LOG_TAG    "fishJni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

CFishEyeCorrect::~CFishEyeCorrect(){}

/*****************************
*将度转为弧度
******************************/

void CFishEyeCorrect::ParamFix()
{
	phi0 = phi0 * piParam;
	sita0 = (90 - sita0) * piParam;
	gama0 = gama0 * piParam;

//	phi1 = phi1 * piParam;
//	sita1 = (90 - sita1) * piParam;
//	gama1 = gama1 * piParam;
//
//	phi2 = phi2 * piParam;
//	sita2 = (90-sita2)*piParam;
//	gama2 = gama2 * piParam;
//
//	phi3 = phi3 * piParam;
//	sita3 = (90-sita3)*piParam;
//	gama3 = gama3 * piParam;
}

/*****************************
*构造函数，初始化piParam
******************************/

CFishEyeCorrect::CFishEyeCorrect():piParam(0.01745329)
{
	
}

/*****************************
*功能：求出源图中鱼眼所在区域
*输入参数：视频输入图
*输出参数：空
*返回值：鱼眼区域所在的矩形
******************************/

Rect CFishEyeCorrect::GetArea(const Mat &inputImage)
{
	FindCorrectArea(inputImage);
	return correctArea;
}

/*****************************
*功能：求出源图中鱼眼所在区域
*输入参数：视频输入图
*输出参数：空
*返回值：空
******************************/

void CFishEyeCorrect::FindCorrectArea(const Mat &inputImage)
{
	Mat grayImage;
	cvtColor(inputImage,grayImage,CV_BGR2GRAY,1);
	/*二值化*/
	threshold(grayImage,grayImage,40,255,THRESH_BINARY);
	vector<vector<Point > > contours;
	/*找轮廓*/
	findContours(grayImage,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	Rect minAreaTemp;
	int areaSize(0); 
	int areaSizeTemp(0);
	int areaIter(0);
	/*找到最大轮廓,就是鱼眼圆的轮廓*/
	for (int contoursIter = 0;contoursIter != contours.size();++contoursIter)
	{
	/*求轮廓的包围盒子*/
		minAreaTemp=boundingRect(contours[contoursIter]);
		areaSizeTemp=minAreaTemp.width*minAreaTemp.height;
		if (areaSize<areaSizeTemp)
		{
			correctArea=minAreaTemp;
			areaSize=areaSizeTemp;
		}
	}
	
}

/*****************************
*功能：计算所需参数
*输入参数：鱼眼区域的宽高
*输出参数：空
*返回值：空
******************************/
void CFishEyeCorrect::CalculateParam(int width, int height)
{
	r=0.5*(width<height?width:height);
	f= r*2/3.14159265;
	cx=(width-1)/2;  
	cy=(height-1)/2;
    std::cout<<"radius "<<r;
}


/*****************************
*功能：展开图在鱼眼区域坐标
*输入参数：展开图中坐标，展开图的方位角，仰角，视角
*输出参数：鱼眼区域中的对应坐标 x y
*返回值：空
******************************/

void CFishEyeCorrect::Fix(int &u,int &v,double &x,double &y,const double &phi,const double &sita,const double &gama)
{
	/*展开平面与校正图比例，平面与空间坐标关系*/
	double xp,yp,zp,SQRT,sit,c_p,s_p;
	//1
	/*展开图像上的一个像素大小对应透视投影平面上的宽高大小*/
		//3
	/*展开图中一个坐标在球面坐标系x,y,z的坐标值*/
	xp=r*sin(sita)*cos(phi)+(cx-u)*dux+(v-cy)*dvx;
	yp=r*sin(sita)*sin(phi)+(cx-u)*duy+(v-cy)*dvy;
	zp=r*cos(sita)+(v-cy)*dvz;
	//4
	/*求出斜边长度*/
	SQRT=sqrt(xp*xp+yp*yp);
	/*求入射角*/
	sit=atan(SQRT/zp);
	/*求cos值*/
	c_p=xp/SQRT;
	/*求sin值*/
	s_p=yp/SQRT;
	//5
	/*对应鱼眼图中坐标*/
	x=cx+f*sit*c_p;
	y=cy+f*sit*s_p;
}

/*****************************
*功能：建立四幅展开图对应的鱼眼图的索引表
*输入参数：鱼眼图
*输出参数：空
*返回值：空
******************************/

void CFishEyeCorrect::CalcPositon() //不需要输入
{
	double x1 = 0 ;
	double y1 = 0;
	int tempPos(0);
    double wp,hp;
    wp=r*tan(gama0/2)/(cx+0.5);
    hp=r*tan(gama0/2)/(cy+0.5);
    //2
    /*展开图U坐标正方向单位像素对应相机坐标系x,y,z三个轴上的变化量*/
    dux=wp*sin(phi0),duy=-wp*cos(phi0),duz=0;
    /*展开图V坐标正方向单位像素对应相机坐标系x,y,z三个轴上的变化量*/
    dvx=-hp*cos(sita0)*cos(phi0),dvy=-hp*cos(sita0)*sin(phi0);
    dvz=hp*sin(sita0);

	for (int row = 0;row < dstHeight ; ++row)
	{
		for (int col = 0;col < dstWidth; ++col)
		{
			int temprow = row * hRatio ;
			int tempcol = col * wRatio;
			tempPos = row*dstWidth*4 + col*2;
			Fix(temprow,tempcol,x1,y1,phi0,sita0,gama0); //得到展开图在鱼眼区域坐标 x1 y1

			//position[tempPos] = (int)x1;
			//position[tempPos + 1] = (int)y1;
            angleDewarpMap_x.at<float>(row,col) = (float)x1 ;
            angleDewarpMap_y.at<float>(row,col) = (float)y1;
		}
	}

}
/*****************************
*功能：鱼眼图校正
*输入参数：鱼眼图像、校正图像
*输出参数：空
*返回值：空
******************************/

void CFishEyeCorrect::NewImageCorrect(const Mat& orgImg, Mat& dstImg)
{
	remap( orgImg, dstImg, angleDewarpMap_x, angleDewarpMap_y, INTER_NEAREST);

    /*//原来的实现
	int orgImgLineWidth = orgImg.step[0];
	int orgImgEleWidth = orgImg.step[1];

	uchar* dstImgData = dstImg.data; 
	uchar* orgImgData = orgImg.data;

	unsigned int tempPos(0);
	unsigned int temOrgPos(0);
	unsigned int temDstPos(0);

	for (int row = 0;row < dstHeight; ++row)
	{
		for (int col = 0;col <dstWidth; ++col)
		{
		    tempPos = ((row * dstWidth)<<2) + (col << 1);// 映射矩阵位置
			temOrgPos = orgImgLineWidth * position[tempPos] + ((position[tempPos + 1]) <<1) + position[tempPos + 1];
			memcpy(dstImgData + temDstPos ,orgImgData+temOrgPos,3);
			temDstPos +=3;
		}
	}
    */
}
//修正位置？positionShift
void CFishEyeCorrect::FixPosition(const Mat& orgImg)
{
	int orgImgLineWidht = orgImg.step[0];
	int orgImgEleWidht = orgImg.step[1];

	long tempPos(0);
	for (int row = 0;row < dstHeight*2; ++row)
	{
		for (int col = 0;col <dstWidth*2; ++col)
		{
			tempPos = ((row * dstWidth)<<2) + (col << 1);
			positionShift[dstWidth*row + col] = (double)orgImgLineWidht*(double)position[tempPos] + (double)((position[tempPos + 1])<<1) + (double)position[tempPos + 1];
		}
	}
}

void CFishEyeCorrect::calcUnfoldPosition()
{
    Rect rect = correctArea;
    int X0 = rect.width/2;
    int Y0 = rect.height/2;

    LOGI("dstRow %d ,dstCol %d \n",dstRow ,dstCol); //dstRow 461 ,dstCol 1449
    int tempPos;
            //目标矩阵V U对应源矩阵X Y。强转成int损失精度。 待把X Y用表记录下来。
    //stereo = Mat( dstRow , dstCol ,origin.type());
            for(int j = 0; j < dstRow;j ++){
                for(int i = 0; i < dstCol ; i++){
                    tempPos=j*dstRow+i*2;

                    double sita = 2 * i / r;
                    int X = X0+j*cos(sita);
                    int Y = Y0+j*sin(sita);

                    unfoldMap_x.at<float>(j,i) = (float)X0+j*cos(sita) ;
                    unfoldMap_y.at<float>(j,i) = (float)Y0+j*sin(sita);

                    //stereo.at<Vec3b>(V,U) = origin.at<Vec3b>(X,Y);  //v:row   u:col
                    //unfoldPosition[tempPos]=Y;
                    //unfoldPosition[tempPos+1]=X;
                }
            }
}

void CFishEyeCorrect::unfold(const Mat& orgImg, Mat& unfoldDstImg){
    //stereo.at<Vec3b>(V,U) = origin.at<Vec3b>(X,Y);  //替换

    remap( orgImg, unfoldDstImg, unfoldMap_x, unfoldMap_y, INTER_NEAREST , BORDER_CONSTANT, Scalar(0, 0, 0) );
    //remap( orgImg, unfoldDstImg, unfoldMap_x, unfoldMap_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0) );
    /*
    int orgImgLineWidth = orgImg.step[0];
    //step[0]是矩阵中一行元素的字节数。

    LOGI("orgImgLineWidth %d  \n",orgImgLineWidth);//4320

    uchar* orgImgData = orgImg.data;
    uchar* dstImgData = unfoldDstImg.data;   //unfoldDstImg

    unsigned int tempPos(0);
    unsigned int temOrgPos(0);
    unsigned int temDstPos(0);

    for(int V = 0; V < dstRow;V ++){
        for(int U = 0; U < dstCol ; U++){
            tempPos = V * dstRow+ U * 2;

            //tempPos = (V * dstCol) + (U << 1);

            //unfoldPosition[tempPos]  x  unfoldPosition[tempPos + 1]  y
            temOrgPos = orgImgLineWidth * unfoldPosition[tempPos] + (unfoldPosition[tempPos + 1]);
            memcpy(dstImgData + temDstPos,orgImgData+temOrgPos,3);
            //void *memcpy(void *dest, const void *src, size_t n);
            //从源src所指的内存地址的起始位置开始拷贝n个字节到目标dest所指的内存地址的起始位置中。
            temDstPos +=3;
        }
    }
    */
}
