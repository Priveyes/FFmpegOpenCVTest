//
// Created by Administrator on 2016/6/30.
//

#include "com_android_lqtian_ffmpegopencvtest_Util.h"
#include <opencv2\opencv.hpp>

//加入调用Android的Log函数的功能。
#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__) //log.e
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)//log.i
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif


JNIEXPORT jlong JNICALL Java_com_android_lqtian_ffmpegopencvtest_Util_decodeToMat  (JNIEnv *env, jobject obj, jstring input_jstr){//obj是调用此方法的对象
    //init
    cv::Mat imgMat;
    cv::Mat *matReturn=new cv::Mat();  // ffmpeg 渲染的格式为 AV_PIX_FMT_BGR24  < packed RGB 8:8:8, 24bpp, BGRBGR...
    char input_str[500]={0};
    sprintf(input_str,"%s",env->GetStringUTFChars(input_jstr, NULL));
    ffmpegDecode ffmpegDecodeObject(input_str);//解码对象

    jclass util=  env->GetObjectClass(obj); //获取util的类，才能继续获取方法
    jmethodID methodId= env->GetMethodID(util,"display","(J)V");

    int i=0;
    while(ffmpegDecodeObject.readOneFrame()>=0){ //读取一帧
            imgMat=ffmpegDecodeObject.getDecodedFrame(); //解码获得Mat，Mat未必非空，不断调用直到非空就有结果。

            if (!imgMat.empty()&&(imgMat.rows>0)){
               //imgMat.copyTo(*matReturn);
               cvtColor(imgMat, imgMat, CV_BGR2RGB); //安卓中的bitmap使用的是RGB。处理完Mat以后进行转换再返回。

                LOGI("readOneFrame call.   %d",i++);   //195
                env->CallVoidMethod(obj,methodId,(jlong)&imgMat);

            }
        }
    return (jlong) matReturn;
}

