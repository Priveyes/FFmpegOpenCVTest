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


    public Util(MySurfaceView mySurfaceView) {
        mMySurfaceView=mySurfaceView;
    }

    public void display(long address){
//        Log.d(TAG, "" + address);//地址是固定的
//        Log.d(TAG, frameMat.toString());//地址是固定的
        if (isFirstStart){
            frameMat = new Mat(address);
            bitmap= Bitmap.createBitmap(frameMat.width(),frameMat.height(), Bitmap.Config.RGB_565);
            mMySurfaceView.setBitmap(bitmap);
            isFirstStart=false;
        }
        //Mat to Bitmap,  显示
//        Log.d(TAG, "" +frameMat.width()+frameMat.height());
        if(frameMat.width()>0 && frameMat.height()>0 && (!frameMat.empty()) ){

            Utils.matToBitmap(frameMat,bitmap);  //准确的话应该每40ms调用一次  15:11:19.330   15:10:53.876
            Log.d(TAG, "mMySurfaceView.setBitmap" );//地址是固定的

            try {
                Thread.sleep(40);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

    }
    public void decode(String inputurl){
        long address=decodeToMat(inputurl);
    }
    public void test(long address){
        Log.d(TAG, "" + address);
    }

    public native long decodeToMat(String inputurl);


}
