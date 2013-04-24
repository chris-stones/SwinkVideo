

#pragma once

#include <libimgutil.h>
#include <stdlib.h>
#include <string.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <vector>
#include <exception>

#include "lz4.h"
#include "lz4hc.h"

#include "PrevFrames.hpp"

template<typename OP_TYPE, int BLOCK_WIDTH, int BLOCK_HEIGHT> 
class BC_ADD_LZ4 {
  
  static const int BLOCK_SIZE = BLOCK_WIDTH * BLOCK_HEIGHT;
  
  int CrCbAdjustResolution(int res,int channel) const {

    switch(channel) {
      case 1:
      case 2:
	return (res+1)>>1;
      default:
	return res;
    }
  }
    
  int blocks(int size, int channel, int blocksize ) const {
    
    int adjustedsize = CrCbAdjustResolution(size, channel);
    
    return ((adjustedsize + (blocksize-1))/blocksize);
  }
  
  struct block_format {
    
    unsigned int channel;
    unsigned int offset;
    unsigned int reserved0;
    unsigned int reserved1;
    
    struct {
      
      OP_TYPE x[BLOCK_WIDTH/sizeof(OP_TYPE)];
      
    } y[BLOCK_HEIGHT];
  };
  
  std::vector<block_format> blockvector;
  void * compressed_data;
  unsigned int compressed_data_size;
  
  bool Process( const imgImage * from, const imgImage * to, int c, int _x, int _y, block_format &bf ) {
   
    const unsigned int stride = blocks( to->width, c, BLOCK_WIDTH ) * BLOCK_WIDTH;
    const unsigned int offset = (stride * BLOCK_HEIGHT * _y) + ( _x * BLOCK_WIDTH ) ;
    
    const OP_TYPE * f = NULL;
    
    if(from)
      f = (const OP_TYPE*)(((const uint8_t *)(from->data.channel[c])) + offset);
    
    const OP_TYPE * t = (const OP_TYPE*)(((const uint8_t *)(to  ->data.channel[c])) + offset);
   
    memset(&bf, 0, sizeof bf);
    
    bf.channel = c;
    bf.offset = offset;

    bool needed = false;
    
    if(f) {
      for( int y=0;y<BLOCK_HEIGHT; y++ ) {
	for( int x=0;x<BLOCK_WIDTH/sizeof(OP_TYPE);x++) {
	  
	  if(f[x]==t[x])
	    continue;
	  
	  needed = true;
	  
	  if(f[x] <= t[x])
	      bf.y[y].x[x] = t[x] - f[x];
	    else
	      bf.y[y].x[x] = (UINT64_MAX - f[x]) + t[x] + 1;
	}
	f += (stride / sizeof(uint64_t));
	t += (stride / sizeof(uint64_t));
      }
    } else {
      for( int y=0;y<BLOCK_HEIGHT; y++ ) {
	for( int x=0;x<BLOCK_WIDTH/sizeof(OP_TYPE);x++) {
	  
	  if(0==t[x])
	    continue;
	  
	  needed = true;
	  
	  bf.y[y].x[x] = t[x];
	}
	t += (stride / sizeof(uint64_t));
      }
    }
    
    return needed;
  }
  
public:
  
  class LZ4Exception : public std::exception {public: const char * what() const throw() { return "LZ4Exception"; } };
  class MEMException : public std::exception {public: const char * what() const throw() { return "MEMException"; } };
  
  typedef OP_TYPE OP_TYPE_T;
  
  BC_ADD_LZ4( PrevFrames & from, const imgImage * to )
    : compressed_data( NULL ),
      compressed_data_size( 0 )
  {
    
    block_format bf;
    
    for(int channel=0; channel<imgGetChannels(to->format); channel++) {
      
      int xb = blocks(to->width, channel, BLOCK_WIDTH);
      int yb = blocks(to->height, channel, BLOCK_HEIGHT );
      
      for(int y=0; y<yb; y++) {
	for(int x=0; x<xb; x++) {
	  
	  if( Process( from.GetLast(), to, channel, x, y, bf ) ) {
	    
	    blockvector.push_back( bf );
	  }
	}
      }
    }
    
    if ( blockvector.size() ) {
      
      int isize = blockvector.size() * sizeof( block_format );
      
      if(!(compressed_data = malloc ( LZ4_compressBound( isize ) ) ))
	throw MEMException();
      
      compressed_data_size = LZ4_compressHC( (const char *)&blockvector[0], (char *)compressed_data, isize );
      
      if( compressed_data_size < 0 ) {
	free(compressed_data);
	compressed_data = NULL;
	throw LZ4Exception();
      }
    }
    
    // TODO: actually the block array over 'to' before setting it as last
    //	to keep track of inperfect transforms, and prevent errors accumulating!
    
    from.SetLast( to );
  }
  
  virtual ~BC_ADD_LZ4() {
  
    free( compressed_data );
  }
  
  int GetNumberOfBlocks() const {
   
    return blockvector.size();
  }
  
  const void * GetCompressedBuffer() const {
    
    return compressed_data;
  }
  
  unsigned int GetCompressedBufferLength() const {
   
    return compressed_data_size;
  }
  
  unsigned int GetUncompressedBufferLength() const {
   
    return sizeof(block_format) * blockvector.size();
  }
};

