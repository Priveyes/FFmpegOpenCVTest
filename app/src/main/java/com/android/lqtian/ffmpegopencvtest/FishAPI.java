package com.android.lqtian.ffmpegopencvtest;

/**
 * Created by apple on 16/6/20.
 */
public class FishAPI {

    static{
        System.loadLibrary("opencv_java3");
        System.loadLibrary("fish_api");
    }

    /**
     * 展开一个鱼眼图
     * @param src
     * @param dst
     */
    public  native void calibrate(long src,long dst);
    //---------------------------------------------------------------------------------------------------

    public native void initUnfoldParam(long src,long dst);

    public native void unfold(long src,long dst);

    //---------------------------------------------------------------------------------------------------
    /**
     * 根据sample计算参数
     * @param src 图片
     */
    public native void initParam(long src,int dstWidth,int dstHeight);

    /**
     * 调整鱼眼的镜头转向
     * @param phi 旋转角
     * @param sita 仰角
     * @param gamma 远近
     */
    public native void faceTo(long src,long dst,double phi,double sita,double gamma);

    //---------------------------------------------------------------------------------------------------

    //计算初始化参数
    public native void initAngleDewarpParam(long src,int dstWidth,int dstHeight);

    //计算映射矩阵
    public native void calculateMap(double phi,double sita,double gamma);
    //进行映射
    public native void map(long src,long dst);

}
