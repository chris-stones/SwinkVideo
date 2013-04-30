
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

#define LOGI(...) ((void)printf(__VA_ARGS__))
#define LOGW(...) ((void)printf(__VA_ARGS__))

static const GLfloat vbuff_data[] = {
//    x     y     s     t
  -1.0f*1.0f, 1.0f*1.0f, 0.0f, 0.0f, // tl
  -1.0f*1.0f,-1.0f*1.0f, 0.0f, 1.0f, // bl
   1.0f*1.0f, 1.0f*1.0f, 1.0f, 0.0f, // tr
   1.0f*1.0f,-1.0f*1.0f, 1.0f, 1.0f, // br
};

GLuint create_vbuffer(GLuint prog) {
  
  GLuint vbuff = 0;
 
  glGenBuffers( 1, &vbuff );
  glBindBuffer(GL_ARRAY_BUFFER, vbuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof vbuff_data, vbuff_data, GL_STATIC_DRAW);
 
  GLuint pos_attr_loc = glGetAttribLocation(prog, "positionAttr");
  glVertexAttribPointer(pos_attr_loc,2,GL_FLOAT,GL_FALSE,4 * sizeof(GLfloat),(const void*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(pos_attr_loc);

  GLuint tex_attr_loc = glGetAttribLocation(prog, "texcoordAttr");
  glVertexAttribPointer(tex_attr_loc,2,GL_FLOAT,GL_FALSE,4 * sizeof(GLfloat),(const void*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(tex_attr_loc);
  
  return vbuff;
}

