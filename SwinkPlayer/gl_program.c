
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <alloca.h>
#include <string.h>
#include <stdio.h>


#define LOGI(...) ((void)printf(__VA_ARGS__))
#define LOGW(...) ((void)printf(__VA_ARGS__))


static GLuint compile_shader( GLenum shaderType, const GLchar * src, int src_len ) {

	GLuint shader = glCreateShader( shaderType );

	glShaderSource( shader, 1, &src, &src_len );
	glCompileShader(shader);

	GLint param = GL_FALSE;

	glGetShaderiv(shader,GL_COMPILE_STATUS, &param);

	if( param != GL_TRUE ) {

		GLint logLength = 0;
		GLint rlogLength = 0;
		GLchar * log = NULL;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		log = (GLchar*)alloca( logLength+1 );
		if(log) {
			memset(log,0,logLength+1);
			glGetShaderInfoLog(shader, logLength, &rlogLength, log);
			LOGI("RH %s",log);
		}

		glDeleteShader(shader);
		shader = 0;
	}

	return shader;
}

static GLint link_program(GLuint program, GLuint vshader, GLuint fshader) {

	GLint linkparam = GL_FALSE;
	GLint validparam = GL_FALSE;

	glAttachShader( program, vshader );
	glAttachShader( program, fshader );
	glLinkProgram( program );
	glGetProgramiv( program, GL_LINK_STATUS, &linkparam );
	glValidateProgram( program );
	glGetProgramiv( program, GL_LINK_STATUS, &validparam );

	if(linkparam == GL_TRUE && validparam == GL_TRUE)
		return GL_TRUE;

	return GL_FALSE;
}

static GLuint create_simple_program(const GLchar * vsource, int vsource_size, const GLchar *fsource, int fsource_size ) {


	GLuint program = 0;
	GLuint vshader = 0;
	GLuint fshader = 0;
	GLint  linkedparam = GL_FALSE;

	program = glCreateProgram();
	vshader = compile_shader( GL_VERTEX_SHADER,   vsource, vsource_size );
	fshader = compile_shader( GL_FRAGMENT_SHADER, fsource, fsource_size );

	if(program && vshader && fshader)
		linkedparam = link_program( program, vshader, fshader);

	if( vshader )
		glDeleteShader( vshader );
	if( fshader )
		glDeleteShader( fshader );

	if(program && (linkedparam != GL_TRUE))
		glDeleteProgram(program);

	if (linkedparam != GL_TRUE)
		program = 0;

	return program;
}

static GLuint _create_ycbcr_program( ) {

	static const GLchar vertex_shader_src[] =
		"attribute vec2 positionAttr;\n"
		"attribute vec2 texcoordAttr;\n"
		"\n"
		"varying vec2 texcoordVar;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vec4(positionAttr.x,positionAttr.y,0.0,1.0);\n"
		"	texcoordVar = texcoordAttr;\n"
		"}\n";


	static const GLchar fragment_shader_src[] =
		"varying vec2 texcoordVar;\n"
		"uniform sampler2D texSamp0;\n"
		"uniform sampler2D texSamp1;\n"
		"uniform sampler2D texSamp2;\n"
		"\n"
		"void main()\n"
		"{\n"
		"  vec3 ycbcr;\n"
		"  vec3 rgb;\n"
		"  float a;\n"
		"  ycbcr.x = texture2D( texSamp0, texcoordVar ).r;\n"
		"  ycbcr.y = texture2D( texSamp1, texcoordVar ).r - 0.5;\n"
		"  ycbcr.z = texture2D( texSamp2, texcoordVar ).r - 0.5;\n"
		"  a       = 1.0;\n"
		"  rgb = mat3(      1,             1,       1,\n"
		"	            0,       -.21482, 2.12798,\n"
		"	            1.28033, -.38059,       0) * ycbcr;\n"
		"  gl_FragColor = vec4(rgb,a);\n"
		"}\n";

	return create_simple_program(
			vertex_shader_src,
			sizeof vertex_shader_src,
			fragment_shader_src,
			sizeof fragment_shader_src
	);
}

static GLuint _create_2d_program( ) {

	static const GLchar vertex_shader_src[] =
		"attribute vec2 positionAttr;\n"
		"attribute vec2 texcoordAttr;\n"
		"\n"
		"varying vec2 texcoordVar;\n"
		"\n"
		"void main(void)\n"
		"{\n"
		"	gl_Position = vec4(positionAttr.x,positionAttr.y,0.0,1.0);\n"
		"	texcoordVar = texcoordAttr;\n"
		"}\n";


	static const GLchar fragment_shader_src[] =
	//	"precision mediump float;\n"
		"varying vec2 texcoordVar;\n"
		"uniform sampler2D texSamp;\n"
		"\n"
		"void main()\n"
		"{\n"
		"  vec4 fragcol =  texture2D(texSamp, texcoordVar);\n"
//		"  vec4 fragcol =  vec4(0.0);\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-1.0/544.0,-1.0/268.0)) * 0.0375;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-0.0/544.0,-1.0/268.0)) * 0.0975;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2( 1.0/544.0,-1.0/268.0)) * 0.0375;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-1.0/544.0,-0.0/268.0)) * 0.0975;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-0.0/544.0,-0.0/268.0)) * 0.5000;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2( 1.0/544.0,-0.0/268.0)) * 0.0975;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-1.0/544.0, 1.0/268.0)) * 0.0375;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2(-0.0/544.0, 1.0/268.0)) * 0.0975;\n"
//		"       fragcol += texture2D(texSamp, texcoordVar + vec2( 1.0/544.0, 1.0/268.0)) * 0.0375;\n"

		"  gl_FragColor = fragcol;\n"
		"}\n";

	return create_simple_program(
			vertex_shader_src,
			sizeof vertex_shader_src,
			fragment_shader_src,
			sizeof fragment_shader_src
	);
}


GLuint create_default_program(  ) {

  GLuint prog = _create_2d_program(  );
  
  GLint texLoc = glGetUniformLocation(prog, "texSamp");

  glUseProgram( prog );
  
  glUniform1i( texLoc , 0);
  
  return prog;	
}

GLuint create_ycbcr_program(  ) {

  GLuint prog = _create_ycbcr_program(  );
  
  glUseProgram( prog );
  
  GLint texLoc0 = glGetUniformLocation(prog, "texSamp0");
  GLint texLoc1 = glGetUniformLocation(prog, "texSamp1");
  GLint texLoc2 = glGetUniformLocation(prog, "texSamp2");

  glUniform1i( texLoc0 , 0);
  glUniform1i( texLoc1 , 1);
  glUniform1i( texLoc2 , 2);
  
  return prog;	
}
