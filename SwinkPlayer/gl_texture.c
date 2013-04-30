
#ifdef TARGET_GLES2
  #include <EGL/egl.h>
  #include <GLES2/gl2.h>
#else
  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glext.h>
#endif

#include <alloca.h>
#include <string.h>
#include <stdio.h>

#include <libimg.h>

#define LOGI(...) ((void)printf(__VA_ARGS__))
#define LOGW(...) ((void)printf(__VA_ARGS__))

#define COMPRESSED_RGB_S3TC_DXT1_EXT  0x83F0
#define COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#define COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#define COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3

static void __report_gl_err(const char * file, const char * func, int line) {

	GLenum e;

	while( (e = glGetError()) != GL_NO_ERROR ) {

		printf("RH %s:%s:%d gl error %d\n", file, func, line, e);
	}
}
#define GL_ERROR() __report_gl_err(__FILE__,__FUNCTION__,__LINE__)

static int CrCbAdjustResolution(int res,int channel) {
  
  switch(channel) {
  case 1:
  case 2:
	  return (res+1)>>1;
  default:
	  return res;
  }
}

static GLenum imgFormatToGlFormat( int fmt ) {
 
  if( ( fmt & IMG_FMT_COMPONENT_PLANAR ) == IMG_FMT_COMPONENT_PLANAR )
//    return GL_RED;
    return GL_LUMINANCE;
  
  if( ( fmt & IMG_FMT_DXT1 ) == IMG_FMT_DXT1 )
    return COMPRESSED_RGBA_S3TC_DXT1_EXT;
  
  if( ( fmt & IMG_FMT_DXT2 ) == IMG_FMT_DXT2 )
    return COMPRESSED_RGBA_S3TC_DXT3_EXT;
  
  if( ( fmt & IMG_FMT_DXT3 ) == IMG_FMT_DXT3 )
    return COMPRESSED_RGBA_S3TC_DXT3_EXT;
  
  if( ( fmt & IMG_FMT_DXT4 ) == IMG_FMT_DXT4 )
    return COMPRESSED_RGBA_S3TC_DXT5_EXT;
  
  if( ( fmt & IMG_FMT_DXT5 ) == IMG_FMT_DXT5 )
    return COMPRESSED_RGBA_S3TC_DXT5_EXT;
  
  return 0;
}

void update_texture( int channel, GLuint tex, int w, int h, void * data, size_t size, int format ) {
  
  GL_ERROR();
  
  glActiveTexture(GL_TEXTURE0 + channel);
  glBindTexture(GL_TEXTURE_2D, tex);
  
  if( format & IMG_FMT_COMPONENT_COMPRESSED ) {
    
    glCompressedTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0,
      0,
      CrCbAdjustResolution(w,channel), 
      CrCbAdjustResolution(h,channel),
      imgFormatToGlFormat( format ),
      size,
      data
    );
  }
  else if( format & IMG_FMT_COMPONENT_PLANAR ) {
    
    glTexSubImage2D(
      GL_TEXTURE_2D,
      0,0,0,
      CrCbAdjustResolution(w,channel), 
      CrCbAdjustResolution(h,channel),
      imgFormatToGlFormat( format ),
      GL_UNSIGNED_BYTE,
      data);
  }
  
  GL_ERROR();
}

void create_texture( int w, int h, int format, GLuint * textures ) {
  
  int channel;
  int channels = imgGetChannels( format );
 
  GL_ERROR();
  glGenTextures(channels, textures);
  

  for(channel=0; channel<channels; channel++) {

    int size = imgGetLinearSize( format, w, h, channel );
    
    glActiveTexture(GL_TEXTURE0 + channel);
    glBindTexture(GL_TEXTURE_2D, textures[channel]);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_ERROR();
    
    if( format & IMG_FMT_COMPONENT_COMPRESSED ) {
      
      glCompressedTexImage2D( 
	GL_TEXTURE_2D,
	0,
	imgFormatToGlFormat( format ),
	CrCbAdjustResolution(w,channel), CrCbAdjustResolution(h,channel), 0,
	size,
	NULL);
    }
    else if( format & IMG_FMT_COMPONENT_PLANAR ) {
    
      glTexImage2D(
	GL_TEXTURE_2D,
	0,
	imgFormatToGlFormat( format ), //1, // internal format
	CrCbAdjustResolution(w,channel), 
	CrCbAdjustResolution(h,channel),
	0,
	imgFormatToGlFormat( format ),
	GL_UNSIGNED_BYTE,
	NULL);
  }
    
    GL_ERROR();
  }
}

