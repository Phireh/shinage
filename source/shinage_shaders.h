#ifndef SHINAGE_SHADERS_H
#define SHINAGE_SHADERS_H

/* TODO: put an #ifdef for Windows / Linux OpenGL locations if needed */
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdlib.h>
#include "shinage_opengl_signatures.h"
#include "shinage_debug.h"
#include "shinage_utils.h"

/* TODO: Add support for compute shaders, etc. in this file's functions */

/* Returns an OpenGL numeric ID to a compiled (but unlinked) shader program. */
unsigned int build_shader(char *source, int type)
{
    char infoLog[512];
    unsigned int shader = openGL.glCreateShader(type);
    openGL.glShaderSource(shader, 1, (const GLchar * const *)(&source), NULL);
    openGL.glCompileShader(shader);
    int success;
    openGL.glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        openGL.glGetShaderInfoLog(shader, 512, NULL, infoLog);
        char *type_str = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        log_err("Error: %s shader compilation failed: %s\n", type_str, infoLog);
    }
    // TODO: Maybe better error handling?
    return shader;
}

/* Returns an OpenGL numeric ID to a compiled (but unlinked) shader program. */
unsigned int build_shader_from_file(char *pathname, int type)
{
    char *file_contents = load_file(pathname);
    unsigned int shader = build_shader(file_contents, type);
    // Cleanup of unneeed buffer
    free(file_contents);
    return shader;
}

/* Takes two pathnames and builds a complete OpenGL program from them, then returns a the ID of said program */
unsigned int make_gl_program(char *pathname_vertex, char *pathname_fragment)
{
    char infoLog[512];
    unsigned int vertex_shader = build_shader_from_file(pathname_vertex, GL_VERTEX_SHADER);
    unsigned int fragment_shader = build_shader_from_file(pathname_fragment, GL_FRAGMENT_SHADER);

    unsigned int program = openGL.glCreateProgram();
    openGL.glAttachShader(program, vertex_shader);
    openGL.glAttachShader(program, fragment_shader);
    openGL.glLinkProgram(program);

    // print linking errors if any
    int success;
    openGL.glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        // TODO: Maybe better error handling?
        openGL.glGetProgramInfoLog(program, 512, NULL, infoLog);
        log_err("Error: shader linking failed: %s\n", infoLog);
    }
    else
    {
        // Cleanup of unneeded structures
        openGL.glDeleteShader(vertex_shader);
        openGL.glDeleteShader(fragment_shader);
    }
    return program;
}

#endif
