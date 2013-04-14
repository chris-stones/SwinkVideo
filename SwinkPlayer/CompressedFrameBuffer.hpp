#include<exception>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include "lz4hc.h"
#include "lz4.h"

class CompressedFrameBuffer {
  
  void * buffer;
  size_t buffer_length;
  
public:
  
  class CompressedBufferException : public std::exception {public: const char * what() const throw() { return "CompressedBufferException"; } };
  
  CompressedFrameBuffer() :
    buffer(NULL),
    buffer_length(0)
  {
  }
  
  CompressedFrameBuffer(size_t initialsize) :
    buffer(NULL),
    buffer_length(0)
  {
    GetBuffer(initialsize);
  }

  virtual ~CompressedFrameBuffer() {
    
    free(buffer);
  }
  
  void * GetBuffer( size_t newsize ) {
    
    if( newsize > buffer_length) {
      
      void * old = buffer;
      buffer = realloc( buffer, newsize );
      if( !buffer ) {
	buffer = old;
	throw CompressedBufferException();
      }
      buffer_length = newsize;
    }
    return buffer;
  }
  
  void * GetBuffer() {
   
    return buffer;
  }
  
  bool Decompress( void * to, size_t to_size ) {
   
    return LZ4_uncompress( (const char *)buffer, (char*)to, to_size) >= 0;
  }
};

