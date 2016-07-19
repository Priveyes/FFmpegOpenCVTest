package com.android.lqtian.ffmpegopencvtest;

import android.app.Activity;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Bundle;

import java.io.IOException;

/**
 * Created by Administrator on 2016/7/13.
 */
public class MediaCodecActivity extends Activity {
    MediaCodec mediaCodec;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        try {
            mediaCodec = MediaCodec.createEncoderByType("Video/AVC");

//        MediaFormat mediaFormat = MediaFormat.createVideoFormat(type, width, height);
//        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 125000);
//        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 15);
//        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);
//        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 5);
//        mediaCodec.configure(mediaFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
//        mediaCodec.start();

        }catch (IOException e) {
            e.printStackTrace();
        }
    }
}
