#ifndef SHINAGE_SHADERS_H
#define SHINAGE_SHADERS_H

#include <GL/glx.h>
#include <GL/glext.h>
#include "shinage_opengl_signatures.h"
#include "shinage_debug.h"
#include "shinage_utils.h"


unsigned int build_shader(char *source, int type)
{
    char infoLog[512];
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar * const *)(&source), NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        char *type_str = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        log_err("Error: %s shader compilation failed: %s\n", type_str, infoLog);
    }
    // TODO: Maybe better error handling?
    return shader;
}

unsigned int build_shader_from_file(char *pathname, int type)
{
    char *file_contents = load_file(pathname);
    unsigned int shader = build_shader(file_contents, type);
    return shader;
}

unsigned int make_gl_program(char *pathname_vertex, char *pathname_fragment)
{
    char infoLog[512];
    unsigned int vertex_shader = build_shader_from_file(pathname_vertex, GL_VERTEX_SHADER);
    unsigned int fragment_shader = build_shader_from_file(pathname_fragment, GL_FRAGMENT_SHADER);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    // print linking errors if any
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)         // TODO: Maybe better error handling?
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        log_err("Error: shader linking failed: %s\n", infoLog);
    }
    else
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
    }
    return program;
}

#endif
