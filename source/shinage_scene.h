#ifndef SHINAGE_SCENE_H
#define SHINAGE_SCENE_H

#include <math.h>
#include "shinage_camera.h"
#include "shinage_debug.h"
#include "shinage_ints.h"

typedef struct
{
	vec4f diffuse;
	vec4f ambient;
	vec4f specular;
	vec4f emissive;
	float shininess;
	int texture_count;
} material_t;

typedef struct {
	vec4f ambient, diffuse, specular;
	vec4f position_world, position_eye;
	vec3f spot_direction_world; // Direction of the spotligh
	bool directional; // Is it a directional light?
	vec3f spot_direction_eye; // // Direction of the spot ligh (camera space)
	bool enabled; // Is it on?
	float spot_exponent, spot_cutoff, spot_cos_cutoff; // Spotlight data
	vec3f attenuation; // kc, kl, kq
} light_source_t;

typedef struct
{
	uint numVertices, _max_vertices;
    vec3f *vertices;
    uint32 *indices;
    vec3f *normals;
    vec2f *tex_coords;
    material_t *material;
    uint *program;
    //model_t* my_model;
    mat4x4f model_mat;
    // If the mesh, the model it belongs to, and the parent of that model do not change
    // there is no need to recalculate the final model_mat for the mesh
    mat4x4f preprocessed_model_mat;
    int model_mat_mismatches;
    bool visible;
    // bool casts_shadows; TODO
} mesh_t;

typedef struct
{
	uint num_meshes, _max_meshes;
    mesh_t *meshes;
    //model_t* parent;
	//uint num_children, _max_children;
    //model_t* children;
    mat4x4f model_mat;
    // If model and its parentdo not change there is no need
    // to recalculate the final model_mat for the model
    mat4x4f preprocessed_model_mat;
    int model_mat_mismatches;
    bool visible;
    // bool casts_shadows; TODO
} model_t;

typedef struct
{
	uint num_models, _max_models;
	model_t *models;
	uint num_light_sources, _max_light_sources;
	light_source_t *light_sources;
    // bool render_shadows; TODO
} scene_t;

/* Get a UV spherical mesh of radius r.
   Adapted from http://www.songho.ca/opengl/gl_sphere.html */
mesh_t sphere_mesh(float r, int nsectors, int nstacks)
{
    if (nsectors < 3)
        nsectors = 32;

    if (nstacks < 2)
        nstacks = 32;

    float stack_step = M_PI / nstacks;
    float sector_step = (2.0f * M_PI) / nsectors;

    vec3f *vectices = malloc(sizeof(vec3f) * nstacks * (nsectors+1));
    vec3f *normals = malloc(sizeof(vec3f) * nstacks * (nsectors+1));
    vec2f *tex_coords = malloc(sizeof(vec2f) * nstacks * (nsectors+1));
    int n = 0;

    /* Create sphere vectices, normals and tex coords */
    for (int i = 0; i < nstacks; ++i)
    {
        float stack_angle = (M_PI / 2.0f) - i * stack_step;
        float xy = r * cosf(stack_angle);
        float z = r * sinf(stack_angle);

        // Add nsectors + 1 vertices per stack
        for (int j = 0; j <= nsectors; ++j, ++n)
        {
            float sector_angle = j * sector_step;

            // Vertex position
            float x = xy * cosf(sector_angle);
            float y = xy * sinf(sector_angle);
            vec3f pos = { .x = x, .y = y, .z = z };
            vectices[n] = pos;

            // Vertex normal
            vec3f normal;
            normal.x = x * (1.0f / r);
            normal.y = y * (1.0f / r);
            normal.z = z * (1.0f / r);
            normals[n] = normal;

            // Vertex tex coordinate
            vec2f tex_coord;
            tex_coord.x = (float)i / nsectors;
            tex_coord.y = (float)j / nstacks;
            tex_coords[n] = tex_coord;
        }
    }

    uint32 *indices = malloc(sizeof(uint32) * nstacks * nsectors);
    n = 0;

    /* Create sphere indices */
    for (int i = 0; i < nstacks; ++i)
    {
        int k1 = i * (nsectors +1);   // beginning of current stack
        int k2 = (k1 * nsectors) + 1; // beginning of next stack

        for (int j = 0; j < nsectors; ++j, ++k1, ++k2)
        {

            /* 2 triangles per sector excluding first and last stacks */
            if (i != 0)
            {
                indices[n++] = k1;
                indices[n++] = k2;
                indices[n++] = k1 + 1;
            }

            if (i != (nstacks - 1))
            {
                indices[n++] = k1 + 1;
                indices[n++] = k2;
                indices[n++] = k2 + 1;
            }

            // NOTE: We could also add line indices for wireframe rendering here
        }

    }

    mesh_t sphere = {};
    sphere.indices = indices;
    sphere.normals = normals;
    sphere.vertices = vectices;
    sphere.tex_coords = tex_coords;
    sphere.visible = true;

    return sphere;
}

/* Convenience function that takes into account the View matrix Z coord
   orientation, see notes on add_translation */
void translate_model(model_t* model, float x, float y, float z)
{
	vec3f aux = { .x = x, .y = y, .z = z };
    model->model_mat = get_translated_matrix_mat4x4f(model->model_mat, aux);
    model->model_mat_mismatches += 1;
}

void rotate_self_model(model_t* model, float pitch, float yaw, float roll)
{
	model->model_mat = get_added_pitch_mat4x4f(model->model_mat, pitch);
    model->model_mat = get_added_yaw_mat4x4f(model->model_mat, yaw);
    model->model_mat = get_added_roll_mat4x4f(model->model_mat, roll);
    model->model_mat_mismatches += 1;
}

void rotate_model(model_t* model, axis3f_t rot_axis, float angle)
{
	model->model_mat = get_rotated_matrix_mat4x4f(model->model_mat, rot_axis, angle);
    model->model_mat_mismatches += 1;
}

void scale_model(model_t* model, float x, float y, float z)
{
	vec3f aux = { .x = x, .y = y, .z = z };
    model->model_mat = get_scaled_matrix_mat4x4f(model->model_mat, aux);
    model->model_mat_mismatches += 1;
}

/* Convenience function that takes into account the View matrix Z coord
   orientation, see notes on add_translation */
void translate_mesh(mesh_t* mesh, float x, float y, float z)
{
	vec3f aux = { .x = x, .y = y, .z = z };
    mesh->model_mat = get_translated_matrix_mat4x4f(mesh->model_mat, aux);
    mesh->model_mat_mismatches += 1;
}

void rotate_self_mesh(mesh_t* mesh, float pitch, float yaw, float roll)
{
	mesh->model_mat = get_added_pitch_mat4x4f(mesh->model_mat, pitch);
    mesh->model_mat = get_added_yaw_mat4x4f(mesh->model_mat, yaw);
    mesh->model_mat = get_added_roll_mat4x4f(mesh->model_mat, roll);
    mesh->model_mat_mismatches += 1;
}

void rotate_mesh(mesh_t* mesh, axis3f_t rot_axis, float angle)
{
	mesh->model_mat = get_rotated_matrix_mat4x4f(mesh->model_mat, rot_axis, angle);
    mesh->model_mat_mismatches += 1;
}

void scale_mesh(mesh_t* mesh, float x, float y, float z)
{
	vec3f aux = { .x = x, .y = y, .z = z };
    mesh->model_mat = get_scaled_matrix_mat4x4f(mesh->model_mat, aux);
    mesh->model_mat_mismatches += 1;
}

void render_scene(scene_t scene)
{
	log_debug("Scene rendering NOT implemented yet: %d objects not rendered", scene.num_models);
	/*uint i, j;
	for (i = 0; i < scene.num_models; i++)
	{
		model_t model = scene.models[i];
		mat4x4f model_mat = model.model_mat;
		for (j = 0; j < model.num_meshes; j++)
		{
			mesh_t mesh = model.meshes[j];
			mat4x4f mesh_mat = mesh.model_mat;
		}
	}*/
}

#endif
