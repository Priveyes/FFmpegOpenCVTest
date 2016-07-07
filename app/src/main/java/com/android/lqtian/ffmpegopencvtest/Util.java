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
    ImageView mImageView; //
    Mat frameMat;
    MySurfaceView mMySurfaceView;
    public Util(ImageView imageView,MySurfaceView mySurfaceView) {
        mImageView = imageView;
        mMySurfaceView=mySurfaceView;
    }

    public void display(long address){
        frameMat = new Mat(address);
//        Log.d(TAG, "" + address);
//        Log.d(TAG, frameMat.toString());

        //Mat to Bitmap,  显示
        Log.d(TAG, "" +frameMat.width()+frameMat.height() );
        if(frameMat.width()>0 && frameMat.height()>0 && (!frameMat.empty()) ){
            Bitmap bitmap= Bitmap.createBitmap(frameMat.width(),frameMat.height(), Bitmap.Config.RGB_565);
            Utils.matToBitmap(frameMat,bitmap);    // OpenCV Error: Assertion failed (src.dims == 2 && info.height == (uint32_t)src.rows && info.width == (uint32_t)src.cols)
//            mImageView.setImageBitmap(bitmap);
            mMySurfaceView.setBitmap(bitmap);
            try {
                Thread.sleep(30);
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
