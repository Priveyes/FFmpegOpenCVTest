/**
 * Simplest FFmpeg Android Decoder
 *
 * ������ Lei Xiaohua
 * leixiaohua1020@126.com
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 *
 * This software is the simplest decoder based on FFmpeg in Android. It can decode video stream
 * to raw YUV data.
 *
 */


#include <stdio.h>
#include <time.h> 

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, "(>_<)", format, ##__VA_ARGS__) //log.e
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  "(=_=)", format, ##__VA_ARGS__)//log.i
#else
#define LOGE(format, ...)  printf("(>_<) " format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf("(^_^) " format "\n", ##__VA_ARGS__)
#endif


//Output FFmpeg's av_log()
void custom_log(void *ptr, int level, const char* fmt, va_list vl){
	FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
	if(fp){
		vfprintf(fp,fmt,vl);
		fflush(fp);
		fclose(fp);
	}
}

JNIEXPORT jint JNICALL Java_com_android_lqtian_ffmpegopencvtest_MainActivity_decode
  (JNIEnv *env, jobject obj, jstring input_jstr, jstring output_jstr)
{
    /*
    定义AVCodec，AVCodec *变量为解码器指针。
    定义AVCodecContext，使用该变量可以将其定义为ffmpeg解码类的类成员。
    定义AVFrame，AVFrame描述一个多媒体帧。解码后的数据将被放在其中。
    定义AVFormatContext变量，AVFormatContext用于保存视频流的有效信息。
    */
    AVCodec			*pCodec;
    AVCodecContext	*pCodecCtx;
	AVFormatContext	*pFormatCtx;
	AVFrame	*pFrame,*pFrameYUV;
	int i, videoindex;

	uint8_t *out_buffer;
	AVPacket *packet;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	FILE *fp_yuv;
	int frame_cnt;
	clock_t time_start, time_finish;
	double  time_duration = 0.0;

    //文件路径
	char input_str[500]={0};
	char output_str[500]={0};
	char info[1000]={0};
	sprintf(input_str,"%s",(*env)->GetStringUTFChars(env,input_jstr, NULL));
	sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,output_jstr, NULL));

	//FFmpeg av_log() callback
    av_log_set_callback(custom_log);

	/*
	初始化libavformat/libavcodec：
    ffmpeg注册复用器，编码器等的函数av_register_all()。
	*/
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();


    // 打开视频文件
	if(avformat_open_input(&pFormatCtx,input_str,NULL,NULL)!=0){
		LOGE("Couldn't open input stream.\n");
		return -1;
	}
	// 取出流信息 保存在pFormatCtx中。
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		LOGE("Couldn't find stream information.\n");
		return -1;
	}
	//遍历文件的各个流，找到第一个视频流，并记录该流的编码信息
	videoindex=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++)
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
			videoindex=i;
			break;
		}
	if(videoindex==-1){
		LOGE("Couldn't find a video stream.\n");
		return -1;
	}
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;

    //在库里面查找支持该格式的解码器
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		LOGE("Couldn't find Codec.\n");
		return -1;
	}
	//打开解码器
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		LOGE("Couldn't open codec.\n");
		return -1;
	}
	//给视频帧分配空间，以便存储解码后的图片数据
	pFrame=av_frame_alloc();
	pFrameYUV=av_frame_alloc();
	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
	pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


  sprintf(info,   "[Input     ]%s\n", input_str);
  sprintf(info, "%s[Output    ]%s\n",info,output_str);
  sprintf(info, "%s[Format    ]%s\n",info, pFormatCtx->iformat->name);
  sprintf(info, "%s[Codec     ]%s\n",info, pCodecCtx->codec->name);
  sprintf(info, "%s[Resolution]%dx%d\n",info, pCodecCtx->width,pCodecCtx->height);


  fp_yuv=fopen(output_str,"wb+");
  if(fp_yuv==NULL){
		printf("Cannot open output file.\n");
		return -1;
	}

	frame_cnt=0;
	time_start = clock();
	//通过该api读入一帧
	while(av_read_frame(pFormatCtx, packet)>=0){
		if(packet->stream_index==videoindex){
		//通过下面的api进行解码一帧数据，将有效的图像数据存储到pAvFrame成员变量中
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if(ret < 0){
				LOGE("Decode Error.\n");
				return -1;
			}
			if(got_picture){
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				y_size=pCodecCtx->width*pCodecCtx->height;
				fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
				fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
				fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
				//Output info
				char pictype_str[10]={0};
				switch(pFrame->pict_type){
					case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
				  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
					case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
					default:sprintf(pictype_str,"Other");break;
				}
				LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
				frame_cnt++;
			}
		}
		av_free_packet(packet);
	}
	//flush decoder
	//FIX: Flush Frames remained in Codec
	while (1) {
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
		if (ret < 0)
			break;
		if (!got_picture)
			break;
		sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
			pFrameYUV->data, pFrameYUV->linesize);
		int y_size=pCodecCtx->width*pCodecCtx->height;
		fwrite(pFrameYUV->data[0],1,y_size,fp_yuv);    //Y
		fwrite(pFrameYUV->data[1],1,y_size/4,fp_yuv);  //U
		fwrite(pFrameYUV->data[2],1,y_size/4,fp_yuv);  //V
		//Output info
		char pictype_str[10]={0};
		switch(pFrame->pict_type){
			case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
		  case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
			case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
			default:sprintf(pictype_str,"Other");break;
		}
		LOGI("Frame Index: %5d. Type:%s",frame_cnt,pictype_str);
		frame_cnt++;
	}
	time_finish = clock();
	time_duration=(double)(time_finish - time_start);

	sprintf(info, "%s[Time      ]%fms\n",info,time_duration);
	sprintf(info, "%s[Count     ]%d\n",info,frame_cnt);

	sws_freeContext(img_convert_ctx);

  fclose(fp_yuv);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

