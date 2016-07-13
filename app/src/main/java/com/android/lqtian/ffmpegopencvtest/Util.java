package com.android.lqtian.ffmpegopencvtest;

import android.graphics.Bitmap;
import android.util.Log;
import android.widget.ImageView;

import org.opencv.android.Utils;
import org.opencv.core.Mat;

/**
 * Created by Administrator on 2016/6/30.
 */
public class Util {
    public static final String TAG = "Util";
    static{
        System.loadLibrary("avutil-54");
        System.loadLibrary("swresample-1");
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avformat-56");
        System.loadLibrary("swscale-3");
        System.loadLibrary("postproc-53");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("sffdecoder");

        System.loadLibrary("opencv_java3");
        System.loadLibrary("decodeToMat");
    }
    Mat frameMat;
    MySurfaceView mMySurfaceView;
    Bitmap bitmap;
    private boolean isFirstStart=true;

    long startTime;
    long endTime;
    long durationTime;
    long sleepTime;

    public Util(MySurfaceView mySurfaceView) {
        mMySurfaceView=mySurfaceView;
    }

    //解码完成一帧后的回调
    public void display(long address){
//        Log.d(TAG, "" + address);//地址是固定的
//        Log.d(TAG, frameMat.toString());//地址是固定的
        frameMat = new Mat(address);
        if (isFirstStart){

            bitmap= Bitmap.createBitmap(frameMat.width(),frameMat.height(), Bitmap.Config.RGB_565);
            mMySurfaceView.setBitmap(bitmap);
            isFirstStart=false;
        }
        //Mat to Bitmap,  显示
//        Log.d(TAG, "" +frameMat.width()+frameMat.height());
        if(!frameMat.empty()){

            Utils.matToBitmap(frameMat,bitmap);  //准确的话应该每40ms调用完一次
            Log.d(TAG, "mMySurfaceView.setBitmap" );
            endTime=System.currentTimeMillis();
            durationTime=endTime-startTime;
            sleepTime=40-durationTime;
            if(sleepTime<0){
                sleepTime=0;
            }else{
                //延时
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            Log.d(TAG, "sleepTime :"+sleepTime );

            startTime=System.currentTimeMillis();
        }

    }

    public native void decodeToMat(String inputurl);


}
