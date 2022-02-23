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
	vector<vec3> vertices;
	vector<GLuint> indici;
	vector<vec3> normals;
	vector<vec2> texCoords;
	GLuint vertexArrayObjID;
	GLuint vertexBufferObjID;
	GLuint normalBufferObjID;
	GLuint indicesBufferObjID;
	GLuint uvBufferObjID;
	bool isIndexed;
} MeshP;

typedef struct {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	int shader;
} Material;

typedef struct {
	Material material;
	MeshP mesh;
	vec3 translation;
	vec3 scale;
	float rotation;
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
	float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;