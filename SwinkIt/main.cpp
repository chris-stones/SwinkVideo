
#include<libimg.h>
#include<libimgutil.h>

#include "SwinkFileWriter.hpp"

#include "BC_ADD_LZ4.hpp"

#include "ppplanar.hpp"

#include "config.h"
#include "nc/nc.h"
#include "PrevFrames.hpp"
#include <stdint.h>

/** supported filename extensions for cgt_nc_* */
static const char * const ext[] =
{
        ".PNG",
        ".BMP",
	".TGA",
	".DDS",
        "",
};

static int same_size( imgImage * i1, imgImage * i2 ) {
  
  if( i1->width != i2->width )
    return 0;
  
  if( i1->height != i2->height )
    return 0;
  
  return 1;
}

static int zero_buffers( imgImage * img ) {
 
  memset( img->data.channel[0], 0, img->linearsize[0] );
  memset( img->data.channel[1], 0, img->linearsize[1] );
  memset( img->data.channel[2], 0, img->linearsize[2] );
  memset( img->data.channel[3], 0, img->linearsize[3] );
}

static int compress( arguments_t * args, cgt_nc * nc ) {
 
  int err = 0;
  int frame;

  struct imgImage * srcImage = NULL;
  struct imgImage * dstImage = NULL;

  int w = -1;
  int h = -1;
  
  {
    
    PrevFrames prevFrames;
    
    SwinkFileWriter swinkFileWriter( *args );
    
    PPPlanar ppPlanar;
    
    printf("compressing %s [%d ... %d] ( %d frames )\n", nc->ident, nc->first, nc->last, nc->file_count );
    
    for(frame = nc->first; frame <= nc->last; frame++) {

      if( imgAllocAndReadF( &srcImage, nc->nc, frame ) == 0 ) {
	
	if(!dstImage) {

	  if( ( err = imgAllocImage( &dstImage ) ) != 0)
	    goto err0;

	  dstImage->format = (imgFormat)(args->format);
	  dstImage->width =  ((srcImage->width+31)/32)*32; // 32 byte aligned width - so that CbCr will be 16 byte aligned.
	  dstImage->height = srcImage->height;

	  if( ( err = imgAllocPixelBuffers( dstImage) ) != 0 )
	    goto err0;
	  
	  zero_buffers( dstImage );
	}

	printf("compressing frame %d.\n\tphase 1...\n", frame);
	if( ( err = imguCopyImage( dstImage, srcImage ) ) != 0 )
	  goto err0;

#if(1)
	{
	  printf("\tphase 2...\n");
	  imgImage * diffed = ppPlanar.Add( dstImage );
	  printf("\tphase 3...\n");
	  swinkFileWriter.WriteFrame( diffed );
	  imgFreeAll( diffed );
	}
#else
	{
	    BC_ADD_LZ4<uint64_t,16,16> BC(prevFrames, dstImage);
	    
	    swinkFileWriter.WriteLZ4BlockArray(
	      BC.GetNumberOfBlocks(),
	      BC.GetCompressedBuffer(), 
	      BC.GetCompressedBufferLength(),
	      BC.GetUncompressedBufferLength(),
	      dstImage);
	}
#endif	
	
	
	
	
	printf("\tdone.\n");

	imgFreeAll( srcImage );
	srcImage = NULL;
      }
    }
  }
  
err0:
  if(err)
    printf("ERROR\n");
//  imgFreeAll(dstImage);
  imgFreeAll(srcImage);

  return err;
}

int main(int argc, char *argv[]) {
 
  arguments_t args = read_args( argc, argv );
  
  cgt_nc_handle	handle = cgt_opennc( args.resources, ext, 0);
  
  cgt_nc * nc = cgt_readnc(handle);
  
  if( nc ) {
    
     compress( &args, nc );
     
  } else {
    
      printf("couldnt find an animation!\n");
  }
 
  cgt_closenc( handle );
  
  return 0;
}

