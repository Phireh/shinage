#ifndef SHINAGE_SCENE_H
#define SHINAGE_SCENE_H

#include <math.h>
#include "shinage_camera.h"
#include "shinage_debug.h"

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
    vec3f* vertices;
    vec3f* indices;
    vec3f* normals;
    material_t* material;
    uint* program;
    model_t* my_model;
    mat4x4f model_mat;
    // If the mesh, the model it belongs to, and the parent of that model do not change
    // there is no need to recalculate the final model_mat for the mesh
    mat4x4f preprocessed_model_mat;
    int model_mat_mismaatches;
    bool visible;
    // bool casts_shadows; TODO
} mesh_t;

typedef struct
{
	uint num_meshes, _max_meshes;
    mesh_t* meshes;
    model_t* parent;
	uint num_children, _max_children;
    model_t* children;
    mat4x4f model_mat;
    // If model and its parentdo not change there is no need
    // to recalculate the final model_mat for the model
    mat4x4f preprocessed_model_mat;
    int model_mat_mismaatches;
    bool visible;
    // bool casts_shadows; TODO
} model_t;

typedef struct
{
	uint num_models, _max_models;
	model_t* models;
	uint num_light_sources, _max_light_sources;
	light_source_t* light_sources;
    // bool render_shadows; TODO
} scene_t;

/* Convenience function that takes into account the View matrix Z coord
   orientation, see notes on add_translation */
void translate_model(model_t* model, float x, float y, float z)
{
	if (!model->model_mat)
		log_err("Error: trying to translate a non initiallized model");
        return;

    vec3f aux = { .x = x, .y = y, .z = z };
    model->model_mat = get_translated_matrix_mat4x4f(model->model_mat, aux);
}

/* Convenience function that takes into account the View matrix Z coord
   orientation, see notes on add_translation */
void translate_mesh(mesh_t* mesh, float x, float y, float z)
{
	if (!mesh->model_mat)
		log_err("Error: trying to translate a non initiallized mesh");
        return;

    vec3f aux = { .x = x, .y = y, .z = z };
    mesh->model_mat = get_translated_matrix_mat4x4f(mesh->model_mat, aux);
}

void rotate_mesh()
{
	
}

void render_scene(scene_t scene)
{
	log_debug("Scene rendering NOT implemented yet: %d objects not rendered", scene.num_models);
	uint i, j;
	for (i = 0; i < scene.num_models; i++)
	{
		model_t model = scene.models[i];
		mat4x4f model_mat = model.model_mat;
		for (j = 0; j < model.num_meshes; j++)
		{
			mesh_t mesh = model.meshes[j];
			mat4x4f mesh_mat = mesh.model_mat;
		}
	}
}

#endif