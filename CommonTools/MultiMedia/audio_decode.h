#ifndef __AUDIO_DECODE_H__
#define __AUDIO_DECODE_H__

/* 
	该文件可以播放mp3,aac等音频格式
	实测可用

	本文件可以作为一个线程使用 decode_audio 为线程入口函数

	基于ffmpeg-3.0.6编译通过
*/

#include <stdlib.h>
#include <string.h>
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "SDL/SDL.h"
};
#include <unistd.h>

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

static  unsigned char  *audio_chunk; 
static  unsigned int  audio_len; 
static  unsigned char  *audio_pos; 

/* 
 * SDL回调函数(音频播放函数)
*/ 
void  fill_audio(void *udata,Uint8 *stream,int len)
{
    static unsigned char audio_buf[(19200/3)*2]={0};
    static unsigned char slice_buf[1024]={0};
    static int audio_buf_size=0;
    static int audio_buf_index=0;
    int len1=0;
    int audio_size=0;
    while(len>0){
        if (audio_buf_index >= audio_buf_size) {
            if(audio_len==0){
                memcpy(audio_buf,slice_buf,1024);
                audio_buf_size=1024;
            }else{
				memcpy(audio_buf,audio_pos,audio_len);
                audio_buf_size = audio_len;
				audio_len=0;
            }
            audio_buf_index = 0;
        }
        len1 = audio_buf_size - audio_buf_index;
        if (len1 > len){
            len1 = len;
        }
        SDL_MixAudio(stream, (unsigned char *)audio_buf + audio_buf_index, len1,SDL_MIX_MAXVOLUME);
        len -= len1;
        stream += len1;
        audio_buf_index += len1;
    }
}


void * decode_audio(void *args)
{
	AVFormatContext	*pFormatCtx;
	int				i, audioStream;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;

	char url[]="/root/test.aac";

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	if(avformat_open_input(&pFormatCtx,url,NULL,NULL)!=0){
		printf("Couldn't open input stream.\n");
		return NULL;
	}
	if(avformat_find_stream_info(pFormatCtx,NULL)<0){
		printf("Couldn't find stream information.\n");
		return NULL;
	}
	
	av_dump_format(pFormatCtx, 0, url, false);

	if(pFormatCtx->nb_streams>10){
		fprintf(stderr,"pFormatCtx->nb_streams %d too large \n",pFormatCtx->nb_streams);
		return NULL;
	}
	audioStream=-1;
	for(i=0; i < pFormatCtx->nb_streams; i++){
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
			audioStream=i;
			break;
		}
	}
	if(audioStream==-1){
		printf("Didn't find a audio stream.\n");
		return NULL;
	}

	pCodecCtx=pFormatCtx->streams[audioStream]->codec;

	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL){
		printf("Codec not found.\n");
		return NULL;
	}

	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
		printf("Could not open codec.\n");
		return NULL;
	}
	AVPacket *packet=(AVPacket *)malloc(sizeof(AVPacket));
	av_init_packet(packet);

	//Out Audio Param
	uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
	//AAC:1024  MP3:1152
	int out_nb_samples=pCodecCtx->frame_size;
	AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;
	int out_sample_rate=44100;
	int out_channels=av_get_channel_layout_nb_channels(out_channel_layout);
	//Out Buffer Size
	int out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,out_nb_samples,out_sample_fmt, 1);

	uint8_t *out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);

	AVFrame	*pFrame;
	pFrame=av_frame_alloc();
	//Init
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {  
		printf( "Could not initialize SDL - %s\n", SDL_GetError()); 
		return NULL;
	}
	//SDL_AudioSpec
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = out_sample_rate; 
	wanted_spec.format = AUDIO_S16SYS; 
	wanted_spec.channels = out_channels; 
	wanted_spec.silence = 0; 
	wanted_spec.samples = out_nb_samples; 
	wanted_spec.callback = fill_audio; 
	wanted_spec.userdata = pCodecCtx; 

	if (SDL_OpenAudio(&wanted_spec,NULL)<0){ 
		printf("can't open audio.\n"); 
		return NULL; 
	} 

	uint32_t ret,len = 0;
	int got_picture;
	int index = 0;
	//FIX:Some Codec's Context Information is missing
	int64_t in_channel_layout=av_get_default_channel_layout(wanted_spec.channels);
	//Swr
	struct SwrContext *au_convert_ctx;
	au_convert_ctx = swr_alloc();
	au_convert_ctx=swr_alloc_set_opts(au_convert_ctx,
										out_channel_layout, 
										out_sample_fmt, 
										out_sample_rate,
										in_channel_layout,
										pCodecCtx->sample_fmt , 
										pCodecCtx->sample_rate,
										0, 
										NULL);
	
	swr_init(au_convert_ctx);

	//Play
	SDL_PauseAudio(0);

	while(av_read_frame(pFormatCtx, packet)>=0){
		if(packet->stream_index==audioStream){

			ret = avcodec_decode_audio4( pCodecCtx, pFrame,&got_picture, packet);
			if ( ret < 0 ) {
                printf("Error in decoding audio frame.\n");
                return NULL;
            }
			if ( got_picture){
				swr_convert(au_convert_ctx,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);
				index++;
			}
			//Set audio buffer (PCM data)
			audio_chunk = (Uint8 *) out_buffer; 
			//Audio buffer length
			audio_len =out_buffer_size;
			audio_pos = audio_chunk;
			while(audio_len>0){//Wait until finish
				SDL_Delay(10); 
			}
		}
		av_free_packet(packet);
	}

	swr_free(&au_convert_ctx);

	SDL_CloseAudio();//Close SDL
	SDL_Quit();

	av_free(out_buffer);
	avcodec_close(pCodecCtx);
	//av_close_input_file(pFormatCtx);

	return NULL;
}

#endif
