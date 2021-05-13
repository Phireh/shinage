#ifndef SHINAGE_SCENE_H
#define SHINAGE_SCENE_H

#include <math.h>
#include "shinage_camera.h"
#include "shinage_debug.h"

typedef struct
{
    vec3f position;
    vec3f scale;
	vec3f rotation;
} transform_t;

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
    material_t material;
    uint* program;
    transform_t transform;
    mat4x4f model_mat;
    int model_mismach;	// Amount of times that the transform has been modified since the last time the model matrix was updated
    bool visible;
    // bool casts_shadows; TODO
} mesh_t;

typedef struct
{
	uint num_meshes, _max_meshes;
    mesh_t* meshes;
    transform_t transform;
    mat4x4f model_mat;
    int model_mismach;	// Amount of times that the transform has been modified since the last time the model matrix was updated
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

mat4x4f get_model_from_transform(transform_t transform)
{
	// TODO: placeholder
	if (transform.position.x)
		return identity_matrix_4x4;
	else
		return identity_matrix_4x4;
}

void render_scene(scene_t scene)
{
	log_debug("Scene rendering NOT implemented yet: %d objects not rendered", scene.num_models);
	uint i, j;
	for (i = 0; i < scene.num_models; i++)
	{
		model_t model = scene.models[i];
		for (j = 0; j < model.num_meshes; j++)
		{
			//mesh_t mesh = model.meshes[j];
		}
	}
}

#endif