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

    ImageView mImageView;
    MySurfaceView mMySurfaceView;
    Util util;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        Button startButton = (Button) this.findViewById(R.id.button_start);
        final EditText urlEdittext_input= (EditText) this.findViewById(R.id.input_url);
//        final EditText urlEdittext_output= (EditText) this.findViewById(R.id.output_url);

        mImageView = (ImageView) findViewById(R.id.imageView);
        mMySurfaceView = (MySurfaceView) findViewById(R.id.mySurfaceView);

        util=new Util(mImageView,mMySurfaceView);

        startButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View arg0){
                String folderurl= Environment.getExternalStorageDirectory().getPath();
                String urltext_input=urlEdittext_input.getText().toString();
                final String inputurl=folderurl+"/"+urltext_input; //输入文件地址String

//                String urltext_output=urlEdittext_output.getText().toString();
//                String outputurl=folderurl+"/"+urltext_output;

                Log.d("inputurl",inputurl);
//                Log.i("outputurl",outputurl);
//                decode(inputurl,outputurl);

                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        util.decode(inputurl);
                    }
                }).start();

//                long address =util.decodeToMat(inputurl);


            }
        });


    }
}
