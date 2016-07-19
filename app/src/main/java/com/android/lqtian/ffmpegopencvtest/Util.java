package com.android.lqtian.ffmpegopencvtest;

import android.graphics.Bitmap;
import android.provider.Settings;
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

    long startTime_fps=0;
    long endTime_fps=0;
    long durationTime_fps=0;

    public Util(MySurfaceView mySurfaceView) {
        mMySurfaceView=mySurfaceView;
    }

    //解码完成一帧后的回调
    public void display(long address,int frame_rate_mils){
//        Log.d(TAG, "" + address);//地址是固定的
//        Log.d(TAG, frameMat.toString());//地址是固定的
//        Log.d(TAG, "frame_rate mils"+frame_rate_mils);
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
            mMySurfaceView.setBitmapUpdated(true);
            //计算两次完成bitmap更新的时间间隔：durationTime_fps
            durationTime_fps= System.currentTimeMillis()-startTime_fps;
            startTime_fps=System.currentTimeMillis();
            float fps=1000f/durationTime_fps;
            Log.d(TAG, "frame rate:"+fps);

            endTime=System.currentTimeMillis();//结束计时
            durationTime=endTime-startTime; //startTime是上次延时完的时间。
            sleepTime=frame_rate_mils-durationTime;//计算应该延时的时间
            if(sleepTime>0){  //大于0才应该延时
                //延时
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            Log.d(TAG, "sleepTime :"+sleepTime );

            startTime=System.currentTimeMillis();//开始计时
        }

    }

    public native void decodeToMat(String inputurl);


}
