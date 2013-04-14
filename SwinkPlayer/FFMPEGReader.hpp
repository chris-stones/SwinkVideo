
#pragma once

#include<exception>

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class FFMPEGReader {
  
  AVFormatContext *pFormatCtx;
  AVCodecContext *pCodecCtx;
  int videoStream;
  AVCodec *pCodec;
  AVFrame *pFrame;
  
public:
  
  class InputOpenException : public std::exception {public: const char * what() const throw() { return "InputOpenException"; } };
  
  FFMPEGReader(const char * file)
    :	pFormatCtx(NULL),
	pCodecCtx(NULL),
	videoStream(-1),
	pCodec(NULL),
	pFrame(NULL)
  {
   
    av_register_all();
        
    if(avformat_open_input(&pFormatCtx, file, NULL, NULL)!=0)
      throw InputOpenException();
    
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
      throw InputOpenException();
    
    av_dump_format(pFormatCtx, 0, file, 0);
    
    // Find the first video stream
    for(int i=0; i<pFormatCtx->nb_streams; i++)
      if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
	videoStream=i;
	break;
      }
    if(videoStream==-1)
      throw InputOpenException();

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;
    
    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL)
      throw InputOpenException();
    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
      throw InputOpenException();
    
    // Allocate video frame
    pFrame=avcodec_alloc_frame();
  }
  
  int GetWidth() const {
   
    return pCodecCtx->width;
  }
  
  int GetHeight() const {
   
    return pCodecCtx->height;
  }
  
  const AVFrame * Read() {
    
    int frameFinished;
    AVPacket packet;

    while(av_read_frame(pFormatCtx, &packet)>=0) {
      // Is this a packet from the video stream?
      if(packet.stream_index==videoStream) {
	    // Decode video frame

	avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
	
	// Did we get a video frame?
	if(frameFinished) {
	// Convert the image from its native format to RGB
	    //img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
		//(AVPicture*)pFrame, pCodecCtx->pix_fmt, 
		//	    pCodecCtx->width, pCodecCtx->height);
	    
	    // Save the frame to disk
	    //if(++i<=5)
	    //  SaveFrame(pFrameRGB, pCodecCtx->width, 
	    //		pCodecCtx->height, i);
	    
	    //return true;
	}
      }
	
      // Free the packet that was allocated by av_read_frame
      av_free_packet(&packet);
      
      if( frameFinished )
	return pFrame;
    } 
    
    av_seek_frame(pFormatCtx,videoStream,0,0 );
    
    return NULL;
  }
  
};



