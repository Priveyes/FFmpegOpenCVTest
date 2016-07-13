package com.android.lqtian.ffmpegopencvtest;

import android.graphics.Bitmap;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

import org.opencv.android.Utils;
import org.opencv.core.Mat;


public class MainActivity extends AppCompatActivity {

    //JNI
//    public native int decode(String inputurl, String outputurl);

    MySurfaceView mMySurfaceView;
    Util util;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        Button startButton = (Button) this.findViewById(R.id.button_start);
        final EditText urlEdittext_input= (EditText) this.findViewById(R.id.input_url);
        mMySurfaceView = (MySurfaceView) findViewById(R.id.mySurfaceView);

        util=new Util(mMySurfaceView);

        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0){
                String folderurl= Environment.getExternalStorageDirectory().getPath();
                String urltext_input=urlEdittext_input.getText().toString();
                final String inputurl=folderurl+"/"+urltext_input; //输入文件地址String


                Log.d("inputurl",inputurl);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        util.decodeToMat(inputurl);
//                        util.decodeToMat("http://download.wavetlan.com/SVV/Media/HTTP/H264/Talkinghead_Media/H264_test1_Talkinghead_mp4_480x360.mp4");
                    }
                }).start();

            }
        });


    }
}
