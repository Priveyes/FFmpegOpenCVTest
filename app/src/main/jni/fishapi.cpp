#include <jni.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <android/log.h>
#include <vector>
#include "FishEyeCorrect.h"

#define  LOG_TAG    "fishJni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


/*
    全景展开：
    initUnfoldParam()
    unfold()


*/

using namespace std;
using namespace cv;

extern "C" {

CFishEyeCorrect fishCorrect;
//Rect rect;

const double piParam = 0.01745329;

//生成展开的全景图
void expand(Mat origin,Mat & stereo){
    LOGI("%i,%i",origin.rows,origin.cols);
    //sita = 2U/R
    double R = fishCorrect.r;
    //全景图 长 Pi*R 宽 R
    double pi = 3.14159265;
    int dstRow = int(R);
    int dstCol = int(R*pi);
    fishCorrect.GetArea(origin);
    Rect rect = fishCorrect.correctArea;
    int X0 = rect.x+rect.width/2;
    int Y0 = rect.y+rect.height/2;
    double t = (double)cvGetTickCount();

        //目标矩阵V U对应源矩阵X Y。强转成int损失精度。 待把X Y用表记录下来。
    stereo = Mat( dstRow , dstCol ,origin.type());
        for(int V = 0; V < dstRow;V ++){
            for(int U = 0; U < dstCol ; U++){
                double sita = 2 * U / R;
                int X = X0+V*cos(sita);
                int Y = Y0+V*sin(sita);
                stereo.at<Vec3b>(V,U) = origin.at<Vec3b>(X,Y);
            }
        }
    t = (double)cvGetTickCount() - t;
    LOGI("remap cost %f",t/cvGetTickFrequency()*1e-6); //remap cost 0.118677
    LOGI("correct complete %i * %i",dstCol,dstRow);
//    resize(stereo,stereo,Size(origin.rows,origin.cols),0,0,CV_INTER_LINEAR);
  }

//JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_calibrate(JNIEnv*, jobject, jlong addrSrc, jlong addrFish);

//全景展开
JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_calibrate(JNIEnv* env, jobject jobj, jlong addrSrc, jlong addrFish)
{
    Mat& mSrc  = *(Mat*)addrSrc;
    Mat& mFish = *(Mat*)addrFish;
    LOGI("%i,%i",mSrc.rows,mSrc.cols);
    //计算鱼眼区域，fishCorrect.correctArea
    fishCorrect.GetArea(mSrc);
    Rect rect = fishCorrect.correctArea;
    //根据鱼眼区域的宽高计算参数
    fishCorrect.CalculateParam(rect.width,rect.height);
    LOGI("correctArea : %i,%i",rect.width,rect.height);
    double t = (double)cvGetTickCount();
    //全景展开
    expand(mSrc(rect),mFish);
//    tm.stop();
    t = (double)cvGetTickCount() - t;
    LOGI("expand cost %f",t/cvGetTickFrequency()*1e-6);
//    mFish = mSrc.clone();
}

//Lqtian:全景展开---------------------------------------------------------
JNIEXPORT void JNICALL Java_com_android_lqtian_ffmpegopencvtest_FishAPI_initUnfoldParam(JNIEnv* env, jobject jobj, jlong addrSrc, jlong addrFish)
{
    Mat& mSrc  = *(Mat*)addrSrc;
    Mat& mFish = *(Mat*)addrFish;

    LOGI("%i,%i",mSrc.rows,mSrc.cols);

    //计算鱼眼区域，fishCorrect.correctArea
    fishCorrect.GetArea(mSrc);
    Rect rect = fishCorrect.correctArea;
    //根据鱼眼区域的宽高计算参数
    fishCorrect.CalculateParam(rect.width,rect.height);
    //取出鱼眼区域图片
    fishCorrect.orgImg = mSrc(rect);
//---------------------------上面是必须的
    double R = fishCorrect.r;
    //全景图 长 Pi*R 宽 R
    double pi = 3.14159265;
    fishCorrect.dstRow = int(R);
    fishCorrect.dstCol = int(R*pi);

    fishCorrect.unfoldDstImg = Mat::zeros(Size(fishCorrect.dstCol,fishCorrect.dstRow),CV_8UC3);//也许可以改写成 dst.create( src.size(), src.type() );
    mFish= fishCorrect.unfoldDstImg;
    //分配 unfoldPosition的内存。
    //fishCorrect.unfoldPosition =  (int*)malloc(fishCorrect.dstRow*fishCorrect.dstCol*2*sizeof(int));//分配内存

    fishCorrect.unfoldMap_x.create( fishCorrect.dstRow,fishCorrect.dstCol, CV_32FC1 );//映射X 矩阵
    fishCorrect.unfoldMap_y.create(fishCorrect.dstRow,fishCorrect.dstCol, CV_32FC1 );//映射Y 矩阵

    double t = (double)cvGetTickCount();
    fishCorrect.calcUnfoldPosition();

     t = (double)cvGetTickCount() - t;
     LOGI("calcUnfoldPosition() cost %f ",t/cvGetTickFrequency()*1e-6);//calcUnfoldPosition() cost 0.118356
}

JNIEXPORT void JNICALL Java_com_android_lqtian_ffmpegopencvtest_FishAPI_unfold(JNIEnv* env, jobject jobj, jlong addrSrc, jlong addrFish){
    Mat& mSrc  = *(Mat*)addrSrc;
    Mat& mFish = *(Mat*)addrFish;

    double t = (double)cvGetTickCount();
    fishCorrect.unfold( fishCorrect.orgImg ,fishCorrect.unfoldDstImg);  //cost 0.017741
    t = (double)cvGetTickCount() - t;
    LOGI("unfold() cost %f ",t/cvGetTickFrequency()*1e-6);// 0.043677   INTER_NEAREST unfold() cost 0.011208
    mFish = fishCorrect.unfoldDstImg;
}


//-----------------------------------------------------------------------------------------

//initParam()   faceTo()

//初始化参数
JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_initParam(JNIEnv* env, jobject jobj, jlong addrSrc,jint dstWidth,jint dstHeight)
{
    int expireTime = 1472659200;
    int seconds = time((time_t*)NULL);
    if(seconds>expireTime){
        LOGI("trial expired! please contact hi_snake@163.com for more support");
        return ;
    }

    double t = (double)cvGetTickCount();

    Mat& mSrc  = *(Mat*)addrSrc;
    LOGI("%i,%i",mSrc.rows,mSrc.cols);
    fishCorrect.GetArea(mSrc);
    Rect rect = fishCorrect.correctArea;

    fishCorrect.dstWidth = dstWidth;
    fishCorrect.dstHeight = dstHeight;
    fishCorrect.wRatio = 5;
    fishCorrect.hRatio = 5;
//    fishCorrect.phi0 = phi;

    fishCorrect.dstImg = Mat::zeros(Size(fishCorrect.dstWidth,fishCorrect.dstHeight),CV_8UC3);
    fishCorrect.position =  (int*)malloc(fishCorrect.dstWidth * fishCorrect.dstHeight * 8 * sizeof(int));
    fishCorrect.positionShift = (long*)malloc(fishCorrect.dstWidth*fishCorrect.dstHeight*4*sizeof(long));

    fishCorrect.orgImg = mSrc(rect);

    //fishCorrect.ParamFix();
    fishCorrect.CalculateParam(fishCorrect.orgImg.cols,fishCorrect.orgImg.rows);
    fishCorrect.CalcPositon();//建立四幅展开图对应的鱼眼图的索引表  fishCorrect.orgImg
    //fishCorrect.FixPosition(fishCorrect.orgImg);

//    fishCorrect.NewImageCorrect(fishCorrect.orgImg,fishCorrect.dstImg);
    t = (double)cvGetTickCount() - t;
    LOGI("initParam cost %f",t/cvGetTickFrequency()*1e-6);
    LOGI("correctArea : %i,%i",rect.width,rect.height);
}

//初始化参数后，对新来的图像进行部分观察
JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_faceTo(JNIEnv* env, jobject jobj, jlong addrSrc, jlong addrFish , jdouble phi,jdouble sita,jdouble gamma)
{
    Mat& mSrc  = *(Mat*)addrSrc;
    Mat& mFish = *(Mat*)addrFish;
    LOGI("%i,%i",mSrc.rows,mSrc.cols);

    double t = (double)cvGetTickCount();

    //fishCorrect.ParamFix();   //与下面语句相似
    fishCorrect.phi0 = phi * piParam;
    fishCorrect.sita0= sita * piParam;
    fishCorrect.gama0 = gamma * piParam;

    Rect rect=fishCorrect.correctArea;
    fishCorrect.orgImg = mSrc(rect);

    fishCorrect.CalcPositon( ); //fishCorrect.orgImg
    LOGI("calc position over!");
    fishCorrect.NewImageCorrect( fishCorrect.orgImg ,fishCorrect.dstImg);
    mFish = fishCorrect.dstImg;
    t = (double)cvGetTickCount() - t;
    LOGI("faceTo cost %f",t/cvGetTickFrequency()*1e-6);
}

//lqtian:-------------------------------------------
/*
    initAngleDewarpParam()
    calculateMap()
    map()
*/
//角度展开
JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_initAngleDewarpParam(JNIEnv* env, jobject jobj, jlong addrSrc,jint dstWidth,jint dstHeight)
{
    int expireTime = 1472659200;
    int seconds = time((time_t*)NULL);
    if(seconds>expireTime){
        LOGI("trial expired! please contact hi_snake@163.com for more support");
        return ;
    }
    double t = (double)cvGetTickCount();

    Mat& mSrc  = *(Mat*)addrSrc;
    LOGI("%i,%i",mSrc.rows,mSrc.cols);
    fishCorrect.GetArea(mSrc);

    fishCorrect.dstWidth = dstWidth;
    fishCorrect.dstHeight = dstHeight;
    fishCorrect.wRatio = 5;
    fishCorrect.hRatio = 5;

    fishCorrect.dstImg = Mat::zeros(Size(fishCorrect.dstWidth,fishCorrect.dstHeight),CV_8UC3);
    //fishCorrect.position =  (int*)malloc(fishCorrect.dstWidth*fishCorrect.dstHeight*8*sizeof(int));

    fishCorrect.angleDewarpMap_x.create( fishCorrect.dstImg.size(), CV_32FC1 );//映射X 矩阵
    fishCorrect.angleDewarpMap_y.create( fishCorrect.dstImg.size(), CV_32FC1 );//映射Y 矩阵

    Rect rect = fishCorrect.correctArea;
    fishCorrect.orgImg = mSrc(rect);
    fishCorrect.CalculateParam(fishCorrect.orgImg.cols,fishCorrect.orgImg.rows);

}

JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_calculateMap(JNIEnv* env, jobject jobj, jdouble phi,jdouble sita,jdouble gamma)
{
    double t = (double)cvGetTickCount();

    //fishCorrect.ParamFix();   //与下面语句相似
    fishCorrect.phi0 = phi * piParam;
    fishCorrect.sita0= sita * piParam;
    fishCorrect.gama0 = gamma * piParam;

    fishCorrect.CalcPositon();

    t = (double)cvGetTickCount() - t;
    LOGI("calculateMap() cost %f",t/cvGetTickFrequency()*1e-6);//calc position() cost 0.144725
}

JNIEXPORT void JNICALL Java_lyan_code_finalndk_FishAPI_map(JNIEnv* env, jobject jobj, jlong addrSrc, jlong addrFish)
{
    Mat& mSrc  = *(Mat*)addrSrc;
    Mat& mFish = *(Mat*)addrFish;

    Rect rect = fishCorrect.correctArea;
    fishCorrect.orgImg = mSrc(rect);

    double t = (double)cvGetTickCount();
    fishCorrect.NewImageCorrect( fishCorrect.orgImg ,fishCorrect.dstImg);
    t = (double)cvGetTickCount() - t;
    LOGI("NewImageCorrect() cost %f",t/cvGetTickFrequency()*1e-6);//NewImageCorrect() cost 0.006853

    mFish = fishCorrect.dstImg;
}


}
