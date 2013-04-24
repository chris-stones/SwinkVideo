#include<exception>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<libimg.h>

#include "lz4hc.h"
#include "lz4.h"

class FrameBuffer {
  
  void * buffer;
  size_t buffer_length;
  
public:
  
  class FrameBufferException : public std::exception {public: const char * what() const throw() { return "FrameBufferException"; } };
  
  FrameBuffer() :
    buffer(NULL),
    buffer_length(0)
  {
  }
  
  FrameBuffer(size_t initialsize) :
    buffer(NULL),
    buffer_length(0)
  {
    GetBuffer(initialsize);
  }

  virtual ~FrameBuffer() {
    
    free(buffer);
  }
  
  void * BufferOffset( unsigned int offset ) {
   
    return static_cast<char*>(buffer) + offset;
  }
  
  void * GetBuffer( imgImage * img ) {
   
    return GetBuffer( img->linearsize[0] + img->linearsize[1] + img->linearsize[2] + img->linearsize[3] );
  }
  
  void * GetBuffer( size_t newsize ) {
    
    if( newsize > buffer_length) {
      
      void * old = buffer;
      buffer = realloc( buffer, newsize );
      if( !buffer ) {
	buffer = old;
	throw FrameBufferException();
      }
      buffer_length = newsize;
    }
    return buffer;
  }
  
  void * GetBuffer() {
   
    return buffer;
  }
}; 

