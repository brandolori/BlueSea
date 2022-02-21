#include <iostream>
#include "Lib.h"
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "geometria.h"
#include "Strutture.h"
#include "objloader.hpp"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace std;

int width = 1480;
int height = 800;
float ycam = 2;
//Inizializzazione Camera
static int selected_obj = 0;
unsigned int idTex, cubemapTexture, texture, texture1;

/*camera structures*/
static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;

typedef enum {
	WIRE_FRAME,
	FACE_FILL,
	CULLING_ON,
	CULLING_OFF,
} MenuOption;

typedef enum {
	GOURAD_SHADING,
	PHONG_SHADING,
	ONDE_SHADING,
	BANDIERA_SHADING,
} ShaderOption;

//Shaders Uniforms 
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders

point_light light;
static vector<Material> materials;
static vector<Shader> shaders;
static vector<Object> objects;
static vector<ObjectP> objectsP;
ObjectP sky = {};

//MATERIALI DI BASE
vec3 red_plastic_ambient = { 0.1, 0.0, 0.0 }, red_plastic_diffuse = { 0.6, 0.1, 0.1 }, red_plastic_specular = { 0.7, 0.6, 0.6 }; GLfloat red_plastic_shininess = 32.0f;
vec3 brass_ambient = { 0.1, 0.06, 0.015 }, brass_diffuse = { 0.78, 0.57, 0.11 }, brass_specular = { 0.99, 0.91, 0.81 }; GLfloat brass_shininess = 27.8f;
vec3 emerald_ambient = { 0.0215, 0.04745, 0.0215 }, emerald_diffuse = { 0.07568, 0.61424, 0.07568 }, emerald_specular = { 0.633, 0.727811, 0.633 }; GLfloat emerald_shininess = 78.8f;
vec3 slate_ambient = { 0.02, 0.02, 0.02 }, slate_diffuse = { 0.1, 0.1, 0.1 }, slate_specular{ 0.4, 0.4, 0.4 }; GLfloat slate_shininess = 1.78125f;


mat4 Projection, Model, View;

float angolo = 0;

LightShaderUniform light_unif = {};

vector<string> faces
{
	/*  "posx.jpg",
	  "negx.jpg",
	  "negy.jpg",
	  "posy.jpg",

	   "posz.jpg",
	  "negz.jpg"*/
	  "right.jpg",
	  "left.jpg",
	  "bottom.jpg" ,
	  "top.jpg",
	  "front.jpg",
	  "back.jpg"
};


void genera_texture(void)
{
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* data = stbi_load("muromattoni.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;

		if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		printf("Tutto OK %d %d \n", width, height);
	}
	else
	{
		printf("Errore nel caricare la texture n. 1\n");
	}
	stbi_image_free(data);


	glGenTextures(1, &texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture

	data = stbi_load("minnie.png", &width, &height, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(1);
	if (data)
	{
		GLenum format;
		if (nrChannels == 3)
			format = GL_RGB;

		if (nrChannels == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Errore nel caricare la texture 2\n");
	}
	stbi_image_free(data);


}

unsigned int loadCubemap(vector<std::string> faces)
{
	//Creazione di 3d cubemap
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		//
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		stbi_set_flip_vertically_on_load(true);
		if (data)
		{
			printf("Texture ok %d \n", i);
			GLenum format;
			if (nrChannels == 3)
				format = GL_RGB;

			if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			printf("Errore Lettura Texture %d \n", i);
			stbi_image_free(data);
		}
	}


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void update_cubo(int a)
{
	angolo += 0.1;
	glutTimerFunc(10, update_cubo, 0);
	glutPostRedisplay();
}

void crea_VAO_obj(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->vertexArrayObjID);
	glBindVertexArray(mesh->vertexArrayObjID);

	glGenBuffers(1, &mesh->vertexBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(vec3), mesh->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO delle NORMALI
	glGenBuffers(1, &mesh->normalBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(vec3), mesh->normals.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}
void crea_VAO_Vector(MeshP* mesh)
{

	glGenVertexArrays(1, &mesh->vertexArrayObjID);
	glBindVertexArray(mesh->vertexArrayObjID);

	glGenBuffers(1, &mesh->vertexBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(vec3), mesh->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO delle NORMALI
	glGenBuffers(1, &mesh->normalBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(vec3), mesh->normals.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//Genero , rendo attivo, riempio il VBO delle TEXTURE
	glGenBuffers(1, &mesh->uvBufferObjID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->uvBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(vec2), mesh->texCoords.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);


	//EBO di tipo indici
	glGenBuffers(1, &mesh->indicesBufferObjID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indicesBufferObjID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

// timing

static unsigned int programId, programId1, programId2, MatrixProj, MatModel, MatView, MatViewS, MatModelS, MatrixProjS, MatView_R, MatModel_R, MatrixProj_R, Vec_cameraPos;

unsigned int VAO;
unsigned int VBO, VBO_COLORI, loc_sc, loc_time, locResolution;
float cameraSpeed = 0.05f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
//Struttura con i campi: posizione x,y,z, colore r,g,b,a


unsigned int la, ld, ls, lp, lsh, leye, lscelta;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

vec3 getTrackBallPoint(float x, float y)
{
	//La trackball virtuale può fornire un'interfaccia utente intuitiva 
	//per la rotazione interattiva del mondo. 
	float zTemp;
	glm::vec3 point;
	//map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	//Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	zTemp = 1.0f - pow(point.x, 2.0) - pow(point.y, 2.0);
	point.z = (zTemp > 0.0f) ? sqrt(zTemp) : 0.0;
	return point;
}

void moveCameraForeward()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position += ViewSetup.direction * cameraSpeed;

}

void moveCameraBack()
{
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.position -= ViewSetup.direction * cameraSpeed;
}

void moveCameraLeft()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
		// e muovo la camera a sinistra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_perp = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector));
	ViewSetup.position -= vec4((direzione_perp), .0) * cameraSpeed;
	ViewSetup.target -= vec4((direzione_perp), .0) * cameraSpeed;
}

void moveCameraRight()
{
	//Calcolo la direzione perpendicolare alla direzione della camera e l'alto della camera
	// e muovo la camera a destra lungo questa direzione
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	vec3 direzione_perp = glm::cross(vec3(ViewSetup.direction), glm::vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(direzione_perp, 0);
	ViewSetup.target += vec4(direzione_perp, 0);
}

void INIT_VAO(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_Gouraud.glsl";
	char* fragmentShader = (char*)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);

	vertexShader = (char*)"vertexShader_CubeMap.glsl";
	fragmentShader = (char*)"fragmentShader_CubeMap.glsl";

	programId1 = ShaderMaker::createProgram(vertexShader, fragmentShader);


	vec3 centro(0.0, 0.0, 0.0);
	vec3 raggio(1.0, 1.0, 1.0);
	genera_texture();
	cubemapTexture = loadCubemap(faces);

	//Creo oggetto sfera
	MeshP sfera = {};
	crea_sfera(centro, raggio, sfera);
	crea_VAO_Vector(&sfera);
	ObjectP obj1 = {};
	obj1.mesh = sfera;
	obj1.material = MaterialType::RED_PLASTIC;
	objectsP.push_back(obj1);
	objectsP[0].name = "SFERA";
	objectsP[0].sceltaS = 6;
	objectsP[0].translation = vec3(0.0, 2.0, 0.0);
	objectsP[0].scale = vec3(3.0, 3.0, 3.0);

	//Creo oggetto Piano suddiviso
	MeshP piano = {};
	crea_piano_suddiviso(piano);
	crea_VAO_Vector(&piano);
	ObjectP obj2 = {};
	obj2.mesh = piano;
	obj2.material = MaterialType::EMERALD;
	objectsP.push_back(obj2);
	objectsP[1].name = "PIANO";
	objectsP[1].sceltaS = 1;

	//Creo oggetto cono
	MeshP cono = {};
	crea_cono(cono);
	crea_VAO_Vector(&cono);
	ObjectP obj3 = {};
	obj3.mesh = cono;
	obj3.material = MaterialType::RED_PLASTIC;
	objectsP.push_back(obj3);
	objectsP[2].name = "CONO";
	objectsP[2].sceltaS = 5;
	objectsP[2].translation = vec3(-7, 5.0, 0.0);
	objectsP[2].scale = vec3(5, 6, 5);

	//Creo oggetto asta
	MeshP asta = {};
	crea_cilindro(asta);
	crea_VAO_Vector(&asta);
	ObjectP obj4 = {};
	obj4.mesh = asta;
	obj4.material = MaterialType::RED_PLASTIC;
	objectsP.push_back(obj4);
	objectsP[3].name = "ASTA";
	objectsP[3].sceltaS = 1;
	objectsP[3].translation = vec3(-16, 0, 0);
	objectsP[3].scale = vec3(.2, 8, .2);

	//Creo oggetto Bandiera
	MeshP bandiera = {};
	crea_piano_suddiviso(bandiera);
	crea_VAO_Vector(&bandiera);
	ObjectP obj5 = {};
	obj5.mesh = piano;
	obj5.material = MaterialType::EMERALD;
	objectsP.push_back(obj5);
	objectsP[4].name = "BANDIERA";
	objectsP[4].sceltaS = 4;
	objectsP[4].translation = vec3(-16, 7, 0);
	objectsP[4].scale = vec3(4, 1, 3);

	//Creo oggetto Pannello
	MeshP pannello = {};
	crea_piano(pannello);
	crea_VAO_Vector(&pannello);
	ObjectP obj5_1 = {};
	obj5_1.mesh = pannello;
	obj5_1.material = MaterialType::EMERALD;
	objectsP.push_back(obj5_1);
	objectsP[5].name = "PANNELLO";
	objectsP[5].sceltaS = 5;
	objectsP[5].translation = vec3(-2, 10, 0);
	objectsP[5].scale = vec3(5, 1, 5);

	// Crea skybox
	MeshP skymesh = {};
	crea_cubo(skymesh);
	crea_VAO_Vector(&skymesh);
	sky.mesh = skymesh;
	sky.material = MaterialType::RED_PLASTIC;

	light.position = { 0.0,5.0,0.0 };
	light.color = { 1.0,1.0,1.0 };
	light.power = 1.f;
	//Definisco il colore che verrà assegnato allo schermo
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// Materials setup
	materials.resize(5);
	materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "Emerald";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "Brass";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SLATE].name = "Slate";
	materials[MaterialType::SLATE].ambient = slate_ambient;
	materials[MaterialType::SLATE].diffuse = slate_diffuse;
	materials[MaterialType::SLATE].specular = slate_specular;
	materials[MaterialType::SLATE].shininess = slate_shininess;

	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	// Shaders setup
	shaders.resize(4);
	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";
	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "PHONG SHADING";
	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "ONDE SHADING";
	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "BANDIERA SHADING";

	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 20.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	Projection = perspective(glm::radians(45.0f), (float)w / float(h), 0.01f, 10000.0f);
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		moveCameraLeft();
		break;

	case 'd':

		moveCameraRight();

		break;

	case 's':

		moveCameraBack();
		break;

	case 'w':

		moveCameraForeward();
		break;
	case 27:
		exit(0);
		break;

	default:
		break;
	}
}

void mouseClick(int button, int state, int x, int y)
{
	glutPostRedisplay();

	glm::vec4 axis;
	float amount = 0.01f;
	// Imposto il valore della trasformazione
	switch (button) {

	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) { moving_trackball = true; }
		if (state == GLUT_UP) { moving_trackball = false; }
		//OperationMode = NAVIGATION;
		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
		break;
	default:
		break;
	}
}

void mouseActiveMotion(int x, int y)
{
	// Spostamento su trackball del vettore posizione Camera 
	if (!moving_trackball) {
		return;
	}
	glm::vec3 destination = getTrackBallPoint(x, y);
	glm::vec3 origin = getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y);
	float dx, dy, dz;
	dx = destination.x - origin.x;
	dy = destination.y - origin.y;
	dz = destination.z - origin.z;
	if (dx || dy || dz) {
		// rotation angle = acos( (v dot w) / (len(v) * len(w)) ) o approssimato da ||dest-orig||;
		float pi = glm::pi<float>();
		float angle = sqrt(dx * dx + dy * dy + dz * dz) * (180.0 / pi);
		// rotation axis = (dest vec orig) / (len(dest vec orig))
		glm::vec3 rotation_vec = glm::cross(origin, destination);
		// calcolo del vettore direzione w = C - A
		ViewSetup.direction = -ViewSetup.target + ViewSetup.position;
		// rotazione del vettore direzione w 
		// determinazione della nuova posizione della camera 
		ViewSetup.position = ViewSetup.target + glm::rotate(mat4(1.0f), glm::radians(-angle), rotation_vec) * ViewSetup.direction;
	}
	last_mouse_pos_X = x; last_mouse_pos_Y = y;
	glutPostRedisplay();
}

void drawScene(void)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Passo al Vertex Shader il puntatore alla matrice Projection, che sarà associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatrixProj
	mat4 Projection = perspective(PerspectiveSetup.fovY, PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);
	mat4 View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));


	// SKYBOX
	glDepthMask(GL_FALSE);
	glUseProgram(programId1);

	glUniform1i(glGetUniformLocation(programId1, "skybox"), 0);
	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));
	glBindVertexArray(sky.mesh.vertexArrayObjID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, sky.mesh.indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	glUseProgram(programId);

	//definizione Luce: posizione, colore ed intensità
	glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	for (auto& obj : objectsP)
	{
		//rendo attivo il VAO dell'oggetto 0, la sfera
		glUniform1i(lscelta, obj.sceltaS);
		glUniform3f(Vec_cameraPos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
		glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
		glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glBindVertexArray(obj.mesh.vertexArrayObjID);
		//Passaggio allo shader del Materiale Sfera
		glUniform3fv(light_unif.material_ambient, 1, value_ptr(materials[obj.material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, value_ptr(materials[obj.material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, value_ptr(materials[obj.material].specular));
		glUniform1f(light_unif.material_shininess, materials[obj.material].shininess);

		//definisco le matrici di Modellazione
		Model = mat4(1.0);
		Model = translate(Model, obj.translation);
		Model = scale(Model, obj.scale);

		//Passo al Vertex Shader il puntatore alla matrice Model, che sarà associata alla variabile Uniform mat4 Projection
		//all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		//Disegno

		glDrawElements(GL_TRIANGLES, obj.mesh.indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glutSwapBuffers();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (PerspectiveSetup.fovY >= 1.0f && PerspectiveSetup.fovY <= 70.0f)
		PerspectiveSetup.fovY -= 0.01 * dir;
	if (PerspectiveSetup.fovY <= 1.0f)
		PerspectiveSetup.fovY = 1.0f;
	if (PerspectiveSetup.fovY >= 70.0f)
		PerspectiveSetup.fovY = 70.0f;

	glutPostRedisplay();
	return;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboardPressedEvent);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glutMouseFunc(mouseClick);
	glutMouseWheelFunc(mouseWheel);
	glutTimerFunc(10, update_cubo, 0);
	glewExperimental = GL_TRUE;
	glewInit();

	INIT_VAO();
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	MatView = glGetUniformLocation(programId, "View");
	Vec_cameraPos = glGetUniformLocation(programId, "cameraPos");
	MatModelS = glGetUniformLocation(programId1, "Model");
	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader


	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatViewS = glGetUniformLocation(programId1, "View");


	la = glGetUniformLocation(programId, "AmbientProduct");
	ld = glGetUniformLocation(programId, "DiffuseProduct");
	ls = glGetUniformLocation(programId, "SpecularProduct");
	//lp = glGetUniformLocation(programId, "LighPosition");
	lsh = glGetUniformLocation(programId, "Shininess");
	leye = glGetUniformLocation(programId, "eyePosition");
	lscelta = glGetUniformLocation(programId, "sceltaS");
	loc_time = glGetUniformLocation(programId, "time");
	idTex = glGetUniformLocation(programId, "tex0");

	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glCullFace(GL_BACK);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}
