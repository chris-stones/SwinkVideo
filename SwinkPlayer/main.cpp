
#include<rh_window.h>

#ifdef TARGET_GLES2
  #include <EGL/egl.h>
  #include <GLES2/gl2.h>
#else
  #define GL_GLEXT_PROTOTYPES
  #include <GL/gl.h>
  #include <GL/glext.h>
#endif

#include<libimg.h>

#include "SwinkFileReader.hpp"
#include "FFMPEGReader.hpp"

extern "C" {
GLuint create_default_program();
GLuint create_ycbcr_program() ;
void update_texture( int i, GLuint tex, int w, int h, void * data, size_t size, int format );
void create_texture( int w, int h, int format, GLuint * textures  );
GLuint create_vbuffer(GLuint prog);
}

static void __report_gl_err(const char * file, const char * func, int line) {

	GLenum e;

	while( (e = glGetError()) != GL_NO_ERROR ) {

		printf("RH %s:%s:%d gl error %d\n", file, func, line, e);
	}
}
#define GL_ERROR() __report_gl_err(__FILE__,__FUNCTION__,__LINE__)


#include<stdint.h>

typedef uint64_t vadd_type_t;

/*
static void vadd( vadd_type_t * __restrict__ _b0, vadd_type_t * __restrict__ _b1, size_t size ) {
  
  vadd_type_t *b0 = (vadd_type_t *)__builtin_assume_aligned(_b0, 16);
  vadd_type_t *b1 = (vadd_type_t *)__builtin_assume_aligned(_b1, 16);
    
  for(int p=0; p<size/sizeof(vadd_type_t); p++)
    b0[p] += b1[p];
}
*/

void vadd( uint64_t * __restrict__ _b0, uint64_t * __restrict__ _b1, size_t size );

int main(int argc, char ** argv) {
  
   int exitflag = 0;
  
  rh_display_handle display = 0;
  rh_screen_handle  screen = 0;
  rh_window_handle  window = 0;
  rh_window_attr_t  window_attr = 0;
  rh_render_handle  render = 0;
  rh_input_handle   input = 0;
  rh_input_data     input_data = 0;
  
  SwinkFileReader * fileReaderSWINK = NULL;
  FFMPEGReader * fileReaderFFMPEG = NULL;
  
  int w = 0;
  int h = 0;
  imgFormat fmt = IMG_FMT_YUV420P;
  
  if( imgCheckFileExtension(argv[1], ".swk" ) == 0 ) {
    
    fileReaderSWINK = new SwinkFileReader( argv[1] );
    
    w = fileReaderSWINK->GetWidth();
    h = fileReaderSWINK->GetHeight();
    fmt = (imgFormat)fileReaderSWINK->GetFormat();
  }
  else
  {
    fileReaderFFMPEG = new FFMPEGReader( argv[1] );
    w = fileReaderFFMPEG->GetWidth();
    h = fileReaderFFMPEG->GetHeight();
  }
  
  rh_window_attr_create( &window_attr );
  rh_window_attr_seti(window_attr, "w", w);
  rh_window_attr_seti(window_attr, "h", h );
  
  rh_display_create(&display);
  rh_screen_create_default(&screen, display);
  rh_window_create(&window, window_attr, screen);
  if( window_attr )
    rh_window_attr_destroy( window_attr );
  rh_render_create(&render,window, 2,1,0);
  rh_bind_render_window(render, window);
  rh_input_create(&input, window);
  
  glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
  
  CompressedFrameBuffer compressedBuffer;
  
  GLuint tex[4] = {0, };
  int totalFrameSize = 0;

  void * bufferSWINK0 = NULL; 
  void * bufferSWINK1 = NULL; 
  
  bool texLoaded = false;
  
  if( fileReaderSWINK ) {
    
    for(int i=0;i<4;i++)
      totalFrameSize += imgGetLinearSize( fmt ,w,h,i );
    
//    bufferSWINK0 = (void*)malloc( totalFrameSize );
//    bufferSWINK1 = (void*)malloc( totalFrameSize );
      
      posix_memalign(&bufferSWINK0, 16, totalFrameSize);
      posix_memalign(&bufferSWINK1, 16, totalFrameSize);
  }
  
  GLuint gl_program = 0;
  
  if(imgGetChannels( fmt ) == 1 )
    gl_program = create_default_program();
  else
    gl_program = create_ycbcr_program();
    
  GLuint gl_vbuff = create_vbuffer(gl_program );
  
  glClearColor(1.0f,0.0f,1.0f,1.0f);
  
  while(!exitflag) {
  
    while(( input_data = rh_input_get( input ) )) {
    
      switch( rh_input_gettype( input_data ) ) {

	case RH_INPUT_KEYPRESS:
	{
	  rh_input_key_enum_t k;
	  rh_input_getkey(input_data, &k);
	  if( k == RH_INPUT_KEY_ESCAPE)
	    exitflag = 1;
	  break;
	}
      }
    }
    
    //glClear( GL_COLOR_BUFFER_BIT );
    
    if( fileReaderSWINK ) {
     
      const SwinkFileReader::swink_frame_header * frame_header = 
	fileReaderSWINK->ReadFrame( compressedBuffer );
      
      if(frame_header->frame_no) {
	
	compressedBuffer.Decompress(bufferSWINK0, totalFrameSize);
	
	vadd_type_t * b0 = (vadd_type_t*)bufferSWINK0;
	vadd_type_t * b1 = (vadd_type_t*)bufferSWINK1;
	
//	for(int p=0; p<totalFrameSize; p++) {
//	  b1[p] += b0[p];
//	}
	vadd(b1, b0, totalFrameSize);
	
      }
      else {
	compressedBuffer.Decompress(bufferSWINK1, totalFrameSize);
      }
	
      
      if(!texLoaded) {
	
	//create_texture(/*w*/2048, /*h*/2048, fmt, tex );
	create_texture(w, h, fmt, tex );
	texLoaded = true;
      }
      
      for(int i =0; i<imgGetChannels(fmt); i++) {
	update_texture( i, tex[i], w, h, static_cast<char*>(bufferSWINK1) + frame_header->channel[i].uncompressed_offset, frame_header->channel[i].uncompressed_size , fmt );
      }
    }
    else if( fileReaderFFMPEG ) {
     
      const AVFrame * frame = fileReaderFFMPEG->Read();
      
      if(frame) {
	
	if(!texLoaded) {
	  create_texture(frame->linesize[0], h, fmt, tex );
	  texLoaded = true;
	}
	
	for(int i =0; i<imgGetChannels(fmt); i++) {

	  update_texture( i, tex[i], frame->linesize[0], h, frame->data[i], 0 ,fmt);
	}
      }
    }
    
//    getchar();
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      
    GL_ERROR();
   
//    rh_window_swapbuffers( window );
    
    rh_window_swapbuffers( window );
    
    glClear(GL_COLOR_BUFFER_BIT);
  }
  
  rh_input_destroy(input);
  rh_bind_render_window(render, NULL);
  rh_render_destroy(render);
  rh_window_destroy(window);
  rh_screen_destroy(screen);
  rh_display_destroy(display);
  
  return 0;
}

