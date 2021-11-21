#ifndef SHINAGE_GUI_H
#define SHINAGE_GUI_H

#include "shinage_opengl_signatures.h"

typedef enum {
      ROOT  = 0,
    WINDOW  = 1,
    TEXTBOX = 2
} gui_type;

struct gui_element_t;
typedef struct gui_element_t gui_element_t;

struct gui_element_t {
    gui_type type;
    bool absolute_position;
    vec3f pos;   // x, y, Z channel (moving on top of another windows)
    vec2f size;  // width, height
    float alpha; // opacity, from 0 to 1

    uint32 background_tex;    // background texture

    char *title;
    char *content;

    gui_element_t *children;
    uint16 nchildren;
};

typedef struct {
    gui_element_t root;
} gui_t;

void draw_element(gui_element_t *e, vec3f father_pos, float father_alpha);

void draw_gui(gui_t *gui)
{
    // GUI has no father
    draw_element(&gui->root, zero_vec3f, 0.0f);
}

void draw_element(gui_element_t *e, vec3f father_pos, float father_alpha)
{
    switch(e->type)
    {
    case ROOT:
    {
        father_pos = e->pos;
        father_alpha = e->alpha;
    } break;

    case WINDOW:
    {
        // Draw rectangular tex
        // TODO: Actually use the provided tex, not this test code
        if (!e->background_tex)
        {
            // 1x1 texture for our single color
            uint8 texels[3] = { 0x01, 0x01, 0x01 /* orange */ };

            glGenTextures(1, &e->background_tex);
            glBindTexture(GL_TEXTURE_2D, e->background_tex);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);

            // Is this necessary?
            // openGL.glGenerateMipMap(GL_TEXTURE_2D);
        }





        /* X , Y and Z coords are relative to the element's father.
           W (transparency) is also relative, but should be between 0 and 1 so it can be multiplied by its father's transparency.
           Children should always have same or less W than fathers. */

        father_pos.x += e->pos.x;
        father_pos.y += e->pos.y;
        father_pos.z += e->pos.z;
        father_alpha *= e->alpha;

    } break;

    case TEXTBOX:
    {
    } break;

    default:
    {
        log_err("Unknown GUI element type!");
    }
    }

    for (uint16 i = 0; i < e->nchildren; ++i)
    {
        draw_element(&e->children[i], father_pos, father_alpha);
    }
}

#endif
