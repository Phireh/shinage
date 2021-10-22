#ifndef SHINAGE_OPENGL_SIGNATURES_H
#define SHINAGE_OPENGL_SIGNATURES_H

#include <GL/glx.h>
#include <GL/glext.h>

/* OpenGL function pointers */
typedef struct {
    PFNGLUSEPROGRAMPROC              glUseProgram;
    PFNGLGETSHADERIVPROC             glGetShaderiv;
    PFNGLSHADERSOURCEPROC            glShaderSource;
    PFNGLCOMPILESHADERPROC           glCompileShader;
    PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
    PFNGLCREATESHADERPROC            glCreateShader;
    PFNGLCREATEPROGRAMPROC           glCreateProgram;
    PFNGLDELETESHADERPROC            glDeleteShader;
    PFNGLGETPROGRAMIVPROC            glGetProgramiv;
    PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
    PFNGLATTACHSHADERPROC            glAttachShader;
    PFNGLLINKPROGRAMPROC             glLinkProgram;
    PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
    PFNGLGENBUFFERSPROC              glGenBuffers;
    PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
    PFNGLBINDBUFFERPROC              glBindBuffer;
    PFNGLBUFFERDATAPROC              glBufferData;
    PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
    PFNGLVERTEXATTRIBIPOINTERPROC    glVertexAttribIPointer;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
    PFNGLUNIFORM1FPROC               glUniform1f;
    PFNGLUNIFORM3FPROC               glUniform3f;
    PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
    PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
    PFNGLUNIFORM1IPROC               glUniform1i;
    PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
    PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;
    PFNGLBUFFERSUBDATAPROC           glBufferSubData;
    PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
} openGL_function_pointers;

openGL_function_pointers openGL;

#ifdef __linux__

int link_gl_functions(void)
{
    // TODO: Error handling
    openGL.glUseProgram              = (PFNGLUSEPROGRAMPROC)             glXGetProcAddress((const GLubyte *)"glUseProgram");
    openGL.glGetShaderiv             = (PFNGLGETSHADERIVPROC)            glXGetProcAddress((const GLubyte *)"glGetShaderiv");
    openGL.glShaderSource            = (PFNGLSHADERSOURCEPROC)           glXGetProcAddress((const GLubyte *)"glShaderSource");
    openGL.glCompileShader           = (PFNGLCOMPILESHADERPROC)          glXGetProcAddress((const GLubyte *)"glCompileShader");
    openGL.glGetShaderInfoLog        = (PFNGLGETSHADERINFOLOGPROC)       glXGetProcAddress((const GLubyte *)"glGetShaderInfoLog");
    openGL.glCreateShader            = (PFNGLCREATESHADERPROC)           glXGetProcAddress((const GLubyte *)"glCreateShader");
    openGL.glCreateProgram           = (PFNGLCREATEPROGRAMPROC)          glXGetProcAddress((const GLubyte *)"glCreateProgram");
    openGL.glDeleteShader            = (PFNGLDELETESHADERPROC)           glXGetProcAddress((const GLubyte *)"glDeleteShader");
    openGL.glGetProgramiv            = (PFNGLGETPROGRAMIVPROC)           glXGetProcAddress((const GLubyte *)"glGetProgramiv");
    openGL.glGetProgramInfoLog       = (PFNGLGETPROGRAMINFOLOGPROC)      glXGetProcAddress((const GLubyte *)"glGetProgramInfoLog");
    openGL.glAttachShader            = (PFNGLATTACHSHADERPROC)           glXGetProcAddress((const GLubyte *)"glAttachShader");
    openGL.glLinkProgram             = (PFNGLLINKPROGRAMPROC)            glXGetProcAddress((const GLubyte *)"glLinkProgram");
    openGL.glGenVertexArrays         = (PFNGLGENVERTEXARRAYSPROC)        glXGetProcAddress((const GLubyte *)"glGenVertexArrays");
    openGL.glGenBuffers              = (PFNGLGENBUFFERSPROC)             glXGetProcAddress((const GLubyte *)"glGenBuffers");
    openGL.glBindVertexArray         = (PFNGLBINDVERTEXARRAYPROC)        glXGetProcAddress((const GLubyte *)"glBindVertexArray");
    openGL.glBindBuffer              = (PFNGLBINDBUFFERPROC)             glXGetProcAddress((const GLubyte *)"glBindBuffer");
    openGL.glBufferData              = (PFNGLBUFFERDATAPROC)             glXGetProcAddress((const GLubyte *)"glBufferData");
    openGL.glVertexAttribPointer     = (PFNGLVERTEXATTRIBPOINTERPROC)    glXGetProcAddress((const GLubyte *)"glVertexAttribPointer");
    openGL.glVertexAttribIPointer    = (PFNGLVERTEXATTRIBIPOINTERPROC)   glXGetProcAddress((const GLubyte *)"glVertexAttribIPointer");
    openGL.glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glXGetProcAddress((const GLubyte *)"glEnableVertexAttribArray");
    openGL.glUniform1f               = (PFNGLUNIFORM1FPROC)              glXGetProcAddress((const GLubyte *)"glUniform1f");
    openGL.glUniform3f               = (PFNGLUNIFORM3FPROC)              glXGetProcAddress((const GLubyte *)"glUniform3f");
    openGL.glGetUniformLocation      = (PFNGLGETUNIFORMLOCATIONPROC)     glXGetProcAddress((const GLubyte *)"glGetUniformLocation");
    openGL.glUniformMatrix4fv        = (PFNGLUNIFORMMATRIX4FVPROC)       glXGetProcAddress((const GLubyte *)"glUniformMatrix4fv");
    openGL.glUniform1i               = (PFNGLUNIFORM1IPROC)              glXGetProcAddress((const GLubyte *)"glUniform1i");
    openGL.glVertexAttribDivisor     = (PFNGLVERTEXATTRIBDIVISORPROC)    glXGetProcAddress((const GLubyte *)"glVertexAttribDivisor");
    openGL.glDrawArraysInstanced     = (PFNGLDRAWARRAYSINSTANCEDPROC)    glXGetProcAddress((const GLubyte *)"glDrawArraysInstanced");
    openGL.glBufferSubData           = (PFNGLBUFFERSUBDATAPROC)          glXGetProcAddress((const GLubyte *)"glBufferSubData");
    openGL.glGenerateMipmap          = (PFNGLGENERATEMIPMAPPROC)         glXGetProcAddress((const GLubyte *)"glGenerateMipmap");

    return 1;
}
#else
#error "Windows version WIP"
#endif
#endif
