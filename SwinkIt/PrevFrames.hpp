
#pragma once

#include<libimgutil.h>
#include<stdlib.h>
#include<string.h>

class PrevFrames {

  imgImage * last;
  
public:
  
  PrevFrames()
    : last(NULL) 
  {
  }
  
  virtual ~PrevFrames() {
   
    imgFreeAll(last);
  }
  
  void SetLast( const imgImage * img ) {
    
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
  
  imgImage * GetLast() {
    
    return last;
  }
};

