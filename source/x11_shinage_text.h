#ifndef X11_SHINAGE_TEXT_H
#define X11_SHINAGE_TEXT_H
#include "shinage_math.h"
#include "shinage_opengl_signatures.h"
#include "shinage_shaders.h"

/* Font rendering related includes */
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    unsigned int tex;      // ID handle of the glyph texture
    vec2f size;            // size of the glyph
    vec2f bearing;         // offset from baseline
    unsigned int advance;  // offset to advance to next glyph
} character_t;

char *default_font_path = "fonts/OpenSans-Regular.ttf";

char *font_vertex_shader_path = "./shaders/font.vert";
char *font_fragment_shader_path = "./shaders/font.frag";
unsigned int font_program = 0;

FT_Library ft_library;
FT_Face default_face;

character_t charmap[128];

/* Function signatures */
int get_window_width(void);
int get_window_height(void);

/* Loads a Freetype library handle. Returns 0 on success, 1 on error.
   NOTE: For our purposes, this should only be called once. It is possible to
   have multiple instances of Freetype with their own fonts, but we don't need to.
 */
int init_freetype(FT_Library *handle)
{
    int err = FT_Init_FreeType(handle);
    if (err)
    {
        log_debug("Error trying to initialize the FreeType lib");
    }
    return err;
}


/* Loads a FreeType face. Returns 0 on success, 1 on error */
int load_face(FT_Library lib_handle, char *path, FT_Face *new_face)
{
    int err = FT_New_Face(lib_handle, path, 0, new_face);
    if (err)
        log_debug("Error trying to load a Freetype face: %s", path);

    return err;
}

/* Loads a charmap using a FreeType face. Returns 0 on error, or the number of successfully
   loaded characters (useless at the moment, might be useful later)

   TODO: More than 128 first ASCII characters, UTF-8 support
 */
int load_charmap(FT_Face face)
{
    /* Enable blending for text rendering */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);
    // disable byte alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int charcount = 0;
    for (int i = 0; i < 128; ++i)
    {
        if (FT_Load_Char(face, (unsigned char)i, FT_LOAD_RENDER))
        {
            log_debug("Failed to load glyph number %d in font %s", i, face->family_name);
            continue;
        }
        else
        {
            ++charcount;
            /* Generate texture */
            unsigned int face_tex;
            glGenTextures(1, &face_tex);
            glBindTexture(GL_TEXTURE_2D, face_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                         face->glyph->bitmap.width,
                         face->glyph->bitmap.rows,
                         0,
                         GL_RED,
                         GL_UNSIGNED_BYTE,
                         face->glyph->bitmap.buffer);

            /* Set texture options */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            character_t c = {
                .tex = face_tex,
                .size = { .x = face->glyph->bitmap.width, face->glyph->bitmap.rows },
                .bearing = { .x = face->glyph->bitmap_left, face->glyph->bitmap_top },
                .advance = (unsigned int)face->glyph->advance.x
            };
            charmap[i] = c;
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Enable blending for text rendering */
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    return charcount;
}

void render_text(char *text, float x, float y, float scale, vec3f color)
{
    /* Enable blending for text rendering */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    if (!font_program)
        font_program = make_gl_program(font_vertex_shader_path, font_fragment_shader_path);
    glUseProgram(font_program);

    static int pmatrix_uniform_pos = -1;
    if (pmatrix_uniform_pos == -1)
        pmatrix_uniform_pos = glGetUniformLocation(font_program, "projMatrix");

    mat4x4f pmatrix;
    pmatrix = orthogonal_proj_matrix(0.0f, get_window_width(), 0.0f, get_window_height());
    glUniformMatrix4fv(pmatrix_uniform_pos, 1, GL_TRUE, pmatrix.v);

    static int color_uniform_pos = -1;
    if (color_uniform_pos == -1)
        color_uniform_pos = glGetUniformLocation(font_program, "textColor");

    glUniform3f(color_uniform_pos, color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    static unsigned int vao = 0;
    if (!vao)
        glGenVertexArrays(1, &vao);

    static unsigned int vbo = 0;
    if (!vbo)
        glGenBuffers(1, &vbo);

    static bool initialized = false;
    if (!initialized)
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        initialized = true;
    }
    glBindVertexArray(vao);


    char c;
    char *p = text;
    while ((c = *p++))
    {
        /* Iterate through characters of string */
        // TODO: Be a decent human being and use a hashtable
        character_t ch = charmap[(int)c];

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        /* Render glyph texture over quad */
        glBindTexture(GL_TEXTURE_2D, ch.tex);
        /* Update content of VBO memory */
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}


#endif
