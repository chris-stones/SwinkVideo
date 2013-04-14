
#pragma once

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<exception>
#include<libimg.h>
#include<assert.h>

#include "FrameBuffer.hpp"
#include "lz4hc.h"
#include "lz4.h"

#include "config.h"

class SwinkFileWriter {

  struct swink_file_header {
  
    char magic[16];
    int  version;
    int  flags;
    int  w;
    int  h;
    int  frames;
    int  format;
    int  first_header_offset;
  };

  struct swink_frame_header {
    
    int frame_no;
    int frame_offset;
    int next_header_offset;
    
    int compressed_size;
    int uncompressed_size;
    
    struct {
      int uncompressed_offset;
      int uncompressed_size;
    } channel [4];
  };
  
  FILE * file;
  
  struct swink_file_header file_header;
  
  arguments_t args;
  
  int index;
  
public:
  
  class OutputOpenException : public std::exception {public: const char * what() const throw() { return "OutputOpenException"; } };
  class OutputWriteException : public std::exception {public: const char * what() const throw() { return "OutputWriteException"; } };
  class OutputSeekException : public std::exception {public: const char * what() const throw() { return "OutputSeekException"; } };
  class OutputMemoryException : public std::exception {public: const char * what() const throw() { return "OutputMemoryException"; } };
  
  SwinkFileWriter( const arguments_t &args )
    :	file(NULL),
	args(args),
	index(0)
  {
    if(!(file = fopen( args.output_file, "wb" )))
      throw OutputOpenException();
    
    memset(&file_header, 0, sizeof file_header);
  }
  
  virtual ~SwinkFileWriter() throw() {
   
    fclose(file);
  }
  
  void WriteFrame( const imgImage *img ) {
    
    if( index == 0 )
      First( img );
    
 //   assert( imgGetChannels(img->format) == 1 && "todo - implement YCbCr colourspace!" );
    
    int src_len = img->linearsize[0] + img->linearsize[1] + img->linearsize[2] + img->linearsize[3];
    
    FrameBuffer srcBuffer( src_len );
    {
      int offset = 0;
      for(int i=0; i<imgGetChannels(img->format); i++) {
	memcpy( srcBuffer.BufferOffset(offset), img->data.channel[i], img->linearsize[i] );
	offset += img->linearsize[i];
      }
    }
    
    void * cdata = malloc( LZ4_compressBound( src_len ) );
    
    if(!cdata)
      throw OutputMemoryException();
    
    unsigned int csize = LZ4_compressHC((const char *)srcBuffer.GetBuffer(), (char *)(cdata), src_len );
    
    Seek( 0, SEEK_END );
    
    swink_frame_header frame_header;
    memset(&frame_header, 0, sizeof frame_header);
    
    frame_header.frame_no = index++;
    frame_header.frame_offset = ftell(file) + sizeof frame_header;
    frame_header.next_header_offset = frame_header.frame_offset + csize;
    frame_header.compressed_size = csize;
    frame_header.uncompressed_size = src_len;
    frame_header.channel[0].uncompressed_offset = 0;
    frame_header.channel[1].uncompressed_offset = frame_header.channel[0].uncompressed_offset + img->linearsize[0];
    frame_header.channel[2].uncompressed_offset = frame_header.channel[1].uncompressed_offset + img->linearsize[1];
    frame_header.channel[3].uncompressed_offset = frame_header.channel[2].uncompressed_offset + img->linearsize[2];
    frame_header.channel[0].uncompressed_size = img->linearsize[0];
    frame_header.channel[1].uncompressed_size = img->linearsize[1];
    frame_header.channel[2].uncompressed_size = img->linearsize[2];
    frame_header.channel[3].uncompressed_size = img->linearsize[3];
    
    Write( frame_header );
    Write( cdata, csize );
    free(cdata);
    
    file_header.frames = index;
    Seek(0,SEEK_SET);
    Write(file_header);
  }
  
private:
  
  void First( const imgImage *img ) {
    
    memcpy(file_header.magic, "rockhopper.swnk", sizeof file_header.magic);
    file_header.format = img->format;
    file_header.w = img->width;
    file_header.h = img->height;
    file_header.first_header_offset = sizeof file_header;
    
    Seek(0, SEEK_SET);
    Write(file_header);
  }
  
  static const char * WhenceStr(int whence) {
   
    switch(whence) {
      default: return "???";
      case SEEK_SET: return "SEEK_SET";
      case SEEK_CUR: return "SEEK_CUR";
      case SEEK_END: return "SEEK_END";
    }
  }
  
  void Seek( long offset, int whence ) {
   
    if( fseek( file, offset, whence ) != 0 ) {
      
      printf("oops - bad seek %d %s\n", (int)offset, WhenceStr((int)whence));
      
      throw OutputSeekException();
    }
  }
  
  void Write( const void * data, unsigned int size ) {
        
    if(fwrite(data,size,1,file) != 1)
      throw OutputWriteException();  
  }
  
  template<typename _T> void Write( const _T & data ) {
    
    Write(&data,sizeof data);
  }
  
};

