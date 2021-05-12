#ifndef SHINAGE_SCENE_H
#define SHINAGE_SCENE_H

#include <math.h>
#include "shinage_matrix_stack_ops.h"

typedef struct
{
	vec3f rotation;
    vec3f translation;
    vec3f scale;
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
    bool visible;
    // bool casts_shadows; TODO
} mesh_t;

typedef struct
{
	uint num_meshes, _max_meshes;
    mesh_t* meshes;
    transform_t transform;
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

#endif