package com.android.lqtian.ffmpegopencvtest;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.support.v4.view.GestureDetectorCompat;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by Administrator on 2016/7/5.
 */

public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback,GestureDetector.OnGestureListener{
    private String TAG = "MySurfaceView";

    private SurfaceHolder mSurfaceHolder;
    private boolean ThreadFlag;
    private Canvas canvas;
    private Bitmap mBitmap;

    private GestureDetectorCompat mDetector;
    private int zeroPointPosX=0;
    private int zeroPointPosY=0;

    public void setBitmap(Bitmap bitmap) {
        mBitmap = bitmap;
    }

    public void setBitmapUpdated(boolean bitmapUpdated) {
        isBitmapUpdated = bitmapUpdated;
    }

    private boolean isBitmapUpdated=true;

    private Thread mThread = new Thread(new Runnable() {
        @Override
        public void run() {
            while (ThreadFlag) {

                // 锁定画布，得到Canvas对象
                canvas = mSurfaceHolder.lockCanvas();
                canvas.drawColor(Color.WHITE);
                //等待bitmap更新的模式
                while(!isBitmapUpdated){
                }

                if(mBitmap!=null) {
                    canvas.drawBitmap(mBitmap, zeroPointPosX, zeroPointPosY, null);
                    Log.d(TAG, "MySurfaceView   drawBitmap!!!");// 16ms的间隔时间
                    isBitmapUpdated=false;
                }
                if (canvas != null) {
                    // 解除锁定，并提交修改内容，更新屏幕
                    mSurfaceHolder.unlockCanvasAndPost(canvas);
                }
//                try {
//                    Thread.sleep(40);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
            }
        }

    });

    public MySurfaceView(Context context) {
        super(context);
        // 通过SurfaceView获得SurfaceHolder对象
        mSurfaceHolder = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        mSurfaceHolder.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // 通过SurfaceView获得SurfaceHolder对象
        mSurfaceHolder = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        mSurfaceHolder.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        // 通过SurfaceView获得SurfaceHolder对象
        mSurfaceHolder = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        mSurfaceHolder.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        // 通过SurfaceView获得SurfaceHolder对象
        mSurfaceHolder = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        mSurfaceHolder.addCallback(this);
    }

    //Callback带来的接口
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // TODO Auto-generated method stub
        Log.i(TAG, "surfaceChanged");
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.i(TAG, "surfaceCreated");
        ThreadFlag = true;
        mDetector = new GestureDetectorCompat(this.getContext(),this);
        mThread.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.i(TAG, "surfaceDestroyed");
        ThreadFlag = false;
    }


    //OnGestureListener
    @Override
    public boolean onDown(MotionEvent motionEvent) {
        Log.d(TAG,"onDown ");
        return false;
    }

    @Override
    public void onShowPress(MotionEvent motionEvent) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent motionEvent) {
        return false;
    }

    @Override
    public boolean onScroll(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        return false;
    }

    @Override
    public void onLongPress(MotionEvent motionEvent) {
        Log.d(TAG,"onLongPress ");
    }

    @Override
    public boolean onFling(MotionEvent motionEvent, MotionEvent motionEvent1, float v, float v1) {
        Log.d(TAG,"onFling ");
        return false;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mDetector.onTouchEvent(event);
        return super.onTouchEvent(event);
    }
}