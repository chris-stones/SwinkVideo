
#pragma once

#include<stdlib.h>
#include<string.h>

#include "libimgutil.h"

#include<stdint.h>

class PPPlanar {
  
  void ZeroLeastSigBits( imgImage * img ) {
    
    //return ;
    
    for( int c = 0; c<4; c++ ) {
      
      unsigned char * buffer = (unsigned char*)img->data.channel[c];
      
      for( int p=0; p<img->linearsize[c]; p++ ) {
	
	if(c==0)
	  buffer[p] &= (~7);
	else if(c==3)
	  buffer[p] = (buffer[p] >> 3);
	else
	  buffer[p] &= (~7);
      }
    }
  }
  
  uint64_t MassageTo(uint64_t from, uint64_t to ) {
      
    uint64_t out = 0;
    
    uint64_t m = 0xf8;
    
    for( uint64_t i=0;i<8;i++) {
     
      uint64_t f = (from >> (i * 8)) & 255ULL;
      uint64_t t = (to   >> (i * 8)) & 255ULL;
      
      if((f&m)!=(t&m)) {
	t &= m;
	t |= (f & (~m));
      }
      
      out |= (t << (i * 8));
    }
    
    return out;
  }
  
  imgImage * CreateDiff( imgImage * img0, imgImage * img1 ) {
    
    imgImage * img;
    imgAllocImage(&img);
    img->format = img1->format;
    img->width  = img1->width;
    img->height = img1->height;
    imgAllocPixelBuffers(img);
    
    for(int c=0;c<4;c++) {
      
      if(img0) {
	
#if(0)
	unsigned char * buffer  = (unsigned char *)img ->data.channel[c];
	unsigned char * buffer0 = (unsigned char *)img0->data.channel[c];
	unsigned char * buffer1 = (unsigned char *)img1->data.channel[c];
	
	for(int p = 0; p<img->linearsize[c]; p++) {
	  
	  if(buffer0[p] <= buffer1[p])
	    buffer[p] = buffer1[p]-buffer0[p];
	  else
	    buffer[p] = (256 - buffer0[p]) + buffer1[p];
	}
#else
	uint64_t * buffer  = (uint64_t *)img ->data.channel[c];
	uint64_t * buffer0 = (uint64_t *)img0->data.channel[c];
	uint64_t * buffer1 = (uint64_t *)img1->data.channel[c];
	uint64_t type_max = 0xffffffffffffffff;
	
	for(int p = 0; p<img->linearsize[c]/sizeof(uint64_t); p++) {
	  
//	  buffer1[p] = MassageTo(buffer0[p], buffer1[p]);
	    
	  if(buffer0[p] <= buffer1[p]) {
	    
	    uint64_t b0 = buffer0[p];
	    uint64_t b1 = buffer1[p];
	    
	    uint64_t d = buffer1[p] -  buffer0[p];
	    buffer[p]  = d;
	  }
	  else {
	    uint64_t d = (type_max - buffer0[p]) + buffer1[p] + 1;
	    buffer[p] = d;
	  }
	  
	  // recalculate last as re-processed current.
	  buffer1[p] = buffer0[p] + buffer[p];
	}
#endif
      }
      else {
	
	unsigned char * buffer  = (unsigned char *)img ->data.channel[c];
	unsigned char * buffer1 = (unsigned char *)img1->data.channel[c];

	memcpy(buffer,buffer1,img->linearsize[c]);
      }
    }
    
    return img;
  }
  
  void SetLast( imgImage * img ) {
    
    imgFreeAll(last);
    last = NULL;
    imgAllocImage(&last);
    last->format = img->format;
    last->width = img->width;
    last->height = img->height;
    imgAllocPixelBuffers(last);
    for(int i=0;i<4;i++)
      memcpy(last->data.channel[i],img->data.channel[i],img->linearsize[i]);
  }
  
  imgImage * last;

public:
  
  PPPlanar() 
    : last(NULL)
  {
    
  }
  
  virtual ~ PPPlanar() 
  {
    imgFreeAll(last);
  }
  
  imgImage * Add( imgImage * img ) {
    
    ZeroLeastSigBits(img);
     
    imgImage * diffimg = CreateDiff(last,img);
     
    SetLast(img);
    
    return diffimg;
  }
  
};

