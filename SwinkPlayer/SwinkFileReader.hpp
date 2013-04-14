
#include<exception>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>

#include "CompressedFrameBuffer.hpp"

class SwinkFileReader {

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

public:
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
private:
  
  FILE * file;
  
  struct swink_file_header file_header;
  struct swink_frame_header frame_header;
  
  int index;
  
public:
  
  class InputOpenException : public std::exception {public: const char * what() const throw() { return "InputOpenException"; } };
  class InputReadException : public std::exception {public: const char * what() const throw() { return "InputReadException"; } };
  class InputSeekException : public std::exception {public: const char * what() const throw() { return "InputSeekException"; } };
  class InputMemoryException : public std::exception {public: const char * what() const throw() { return "InputMemoryException"; } };
  class InputFormatException : public std::exception {public: const char * what() const throw() { return "InputFormatException"; } };
  
  SwinkFileReader( const char * fn )
    :	file(NULL),
	index(0)
  {
    if(!(file = fopen( fn, "rb" )))
      throw InputOpenException();
    
    Read( file_header );
    
    if( strcmp( file_header.magic, "rockhopper.swnk" ) != 0 )
      throw InputFormatException();
  }
  
  virtual ~SwinkFileReader() throw() {
   
    fclose(file);
  }
  
  int GetWidth() const {
    
    return file_header.w;
  }
  
  int GetFormat() const {
   
    return file_header.format;
  }
  
  int GetHeight() const  {
    
    return file_header.h;
  }
  
  int GetFrames() const {
    
    return file_header.frames;
  }
  
  bool AtEnd() const {
   
    return index >= file_header.frames;
  }
  
  const swink_frame_header * ReadFrame( CompressedFrameBuffer & compressedFrameBuffer ) {
    
    if( !file_header.frames )
      return NULL;
      
    if( index >= file_header.frames )
      index = 0;
   
    Seek( index ? frame_header.next_header_offset : file_header.first_header_offset, SEEK_SET );
    Read( frame_header );
    
//    printf("reader - uncompressed size = %d\n", frame_header.uncompressed_size);
    
    void * buffer = compressedFrameBuffer.GetBuffer(frame_header.compressed_size);
    
    Read( buffer, frame_header.compressed_size );
     
    ++index;
    
    return &frame_header;
  }
  
private:
    
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
      
      throw InputSeekException();
    }
  }
  
  void Read( void * data, unsigned int size ) {
        
    if(fread(data,size,1,file) != 1)
      throw InputReadException();  
  }
  
  template<typename _T> void Read( _T & data ) {
    
    Read(&data,sizeof data);
  }
  
};
