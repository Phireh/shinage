#ifndef SHINAGE_OPENGL_SIGNATURES_H
#define SHINAGE_OPENGL_SIGNATURES_H

#include <GL/glx.h>
#include <GL/glext.h>

/* OpenGL function signatures */
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
PFNGLUNIFORM3FPROC               glUniform3f;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
PFNGLUNIFORM1IPROC               glUniform1i;
PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;

#endif
