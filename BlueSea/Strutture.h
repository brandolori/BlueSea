#pragma once
#include "Lib.h"
#include <string>
typedef struct {
	float x, y, z;
	float r, g, b, a;
} Point;

typedef struct {

	float r, g, b, a;
}ColorRBGA;

typedef struct {
	GLuint i, j, k;
} Indici;

typedef struct {
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	GLuint vertexArrayObjID;
	GLuint vertexBufferObjID;
	GLuint normalBufferObjID;
	GLuint uvBufferObjID;
} Mesh;

typedef struct {
	std::vector<vec3> vertices;
	std::vector <GLuint>  indici;
	std::vector<vec3> normals;
	std::vector<vec2> texCoords;
	GLuint vertexArrayObjID;
	GLuint vertexBufferObjID;
	GLuint normalBufferObjID;
	GLuint indicesBufferObjID;
	GLuint uvBufferObjID;
} MeshP;

typedef enum {
	RED_PLASTIC,
	EMERALD,
	BRASS,
	SLATE,
	NO_MATERIAL
} MaterialType;

typedef struct {
	string name;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat shininess;
} Material;

typedef struct {
	string name;
	int value;
} Shader;

typedef enum { // used also as index, don't modify order
	GOURAUD,
	PHONG,
	BLINN,
	TOON,
	PASS_THROUGH,
	WAVE
} ShadingType;

typedef struct {
	Mesh mesh;
	MaterialType material;
	ShadingType shading;
	mat4 M;
	string name;
} Object;

typedef struct {
	MeshP mesh;
	MaterialType material;
	ShadingType shading;
	mat4 M;
	string name;
	int sceltaS;
	vec3 translation;
	vec3 scale;
} ObjectP;

typedef struct {
	GLuint light_position_pointer;
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	GLuint material_shininess;
} LightShaderUniform;

typedef struct {
	GLuint P_Matrix_pointer;
	GLuint V_Matrix_pointer;
	GLuint M_Matrix_pointer;
	GLfloat time_delta_pointer;
} BaseShaderUniform;

typedef struct {
	vec3 position;
	vec3 color;
	GLfloat power;
} point_light;

struct {
	vec4 position;
	vec4 target;
	vec4 upVector;
	vec4 direction;
} ViewSetup;

struct {
	float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;