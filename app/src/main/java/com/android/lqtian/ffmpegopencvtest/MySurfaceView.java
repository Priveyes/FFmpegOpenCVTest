package com.android.lqtian.ffmpegopencvtest;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by Administrator on 2016/7/5.
 */

public class MySurfaceView extends SurfaceView implements SurfaceHolder.Callback {
    private String TAG = "MySurfaceView";

    private SurfaceHolder sfh;
    private boolean ThreadFlag;
    private int counter;
    private Canvas canvas;
    private Bitmap mBitmap;

    public void setBitmap(Bitmap bitmap) {
        mBitmap = bitmap;
    }

    private Thread mThread = new Thread(new Runnable() {
        @Override
        public void run() {
            while (ThreadFlag) {
                // 锁定画布，得到Canvas对象
                canvas = sfh.lockCanvas();
                if(mBitmap!=null)
                    canvas.drawBitmap(mBitmap,0,0,null);

                if (canvas != null) {
                    // 解除锁定，并提交修改内容，更新屏幕
                    sfh.unlockCanvasAndPost(canvas);
                }
//                try {
//                    Thread.sleep(30);
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
            }
        }

    });

    public MySurfaceView(Context context) {
        super(context);
        // 通过SurfaceView获得SurfaceHolder对象
        sfh = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        sfh.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        // 通过SurfaceView获得SurfaceHolder对象
        sfh = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        sfh.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        // 通过SurfaceView获得SurfaceHolder对象
        sfh = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        sfh.addCallback(this);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes) {
        super(context, attrs, defStyleAttr, defStyleRes);
        // 通过SurfaceView获得SurfaceHolder对象
        sfh = this.getHolder();
        // 为SurfaceHolder添加回调结构SurfaceHolder.Callback
        sfh.addCallback(this);
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
        counter = 0;
        ThreadFlag = true;
        mThread.start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        // TODO Auto-generated method stub
        Log.i(TAG, "surfaceDestroyed");
        ThreadFlag = false;
    }

}