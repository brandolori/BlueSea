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
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;
using namespace std;

int width = 1480;
int height = 800;

//Inizializzazione Camera
unsigned int idTex, cubemapTexture, texture, texture1;

//Shaders Uniforms 
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders

point_light light;
static vector<ObjectP> objectsP;
ObjectP sky = {};

mat4 Projection, Model, View;

float currentTime = 0;

LightShaderUniform light_unif = {};

// timing

static unsigned int programId, programId1, programId2, MatrixProj, MatModel, MatView, MatViewS, MatModelS, MatrixProjS, MatView_R, MatModel_R, MatrixProj_R, Vec_cameraPos;

unsigned int VAO;
unsigned int VBO, VBO_COLORI, loc_sc, loc_time, locResolution;
float cameraSpeed = 0.05f;

//Struttura con i campi: posizione x,y,z, colore r,g,b,a
unsigned int la, ld, ls, lp, lsh, leye, lscelta, lcenter;

// game variables

unsigned int timestep = 1000 / 60;
float floatTimestep = 1.0 / 60.0;

float movementForce = 50.0;
float movementDrag = 25.0;
float maxSpeed = 50;

vec3 cameraOffset = vec3(0, 20, 30);
vec3 cameraTargetOffset = vec3(0, 10, 0);

float range = 1000;

// game state
vec3 speed = vec3(0, 0, 0);
int inputX = 0;
int inputY = 0;
vec2 boatDirection = vec2(0, 0);
vec2 targetDirection = vec2(0, 0);

vector<string> faces
{
	  "posx.jpg",
	  "negx.jpg",
	  "negy.jpg",
	  "posy.jpg",

	  "posz.jpg",
	  "negz.jpg"
	  //"right.jpg",
	  //"left.jpg",
	  //"bottom.jpg" ,
	  //"top.jpg",
	  //"front.jpg",
	  //"back.jpg"
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

unsigned int loadCubemap(vector<string> faces)
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

	//Carica modello obj barca
	MeshP barca = {};
	bool modello = loadOBJ("bout2.obj", barca);
	crea_VAO_obj(&barca);
	ObjectP objBarca = {
		{
			vec3(.1,0,0),
			vec3(.6,.2,.2),
			vec3(.7,.6,.6),
			100.0,
			2
		},
		barca,
		vec3(-10.0, 0.0, 100.0),
		vec3(1, 1, 1),
		0
	};
	objectsP.push_back(objBarca);

	// sfere riflettenti
	for (int i = 0; i < 100; i++) {
		MeshP sfera = {};
		crea_sfera(centro, raggio, sfera);
		crea_VAO_Vector(&sfera);
		ObjectP obj1 = {
			{
				vec3(1,0,0),
				vec3(1,0,0),
				vec3(1,0,0),
				32.0,
				6
			},
			sfera,
			vec3(random_range(-range,range), 0.0, random_range(-range,range)),
			vec3(3.0, 3.0, 3.0),
			0,
			true
		};
		objectsP.push_back(obj1);
	}

	//Creo oggetto Piano suddiviso
	MeshP piano = {};
	crea_piano_suddiviso(512, piano);
	crea_VAO_Vector(&piano);
	ObjectP obj2 = {
		{
			vec3(0,0,0),
			vec3(0,.2,.6),
			vec3(0,0,0),
			32.0,
			3
		},
		piano,
		vec3(-range, 0, -range),
		vec3(range * 2, 1, range * 2),
		0
	};
	objectsP.push_back(obj2);

	// sfere di gomma
	for (int i = 0; i < 100; i++) {
		MeshP sfera = {};
		crea_sfera(centro, raggio, sfera);
		crea_VAO_Vector(&sfera);
		ObjectP obj1 = {
			{
				vec3(random_range(0,0.8),random_range(0,0.8),random_range(0,0.8)),
				vec3(.5,.5,.5),
				vec3(.5,.5,.5),
				100.0,
				2
			},
			sfera,
			vec3(random_range(-range,range), 0.0, random_range(-range,range)),
			vec3(3.0, 3.0, 3.0),
			0,
			true
		};
		objectsP.push_back(obj1);
	}

	// Crea skybox
	MeshP skymesh = {};
	crea_cubo(skymesh);
	crea_VAO_Vector(&skymesh);
	sky.mesh = skymesh;
	sky.material = {};

	light.position = { 0.0, 1000, 0.0 };
	light.color = { 1.0, 1.0, 1.0 };
	light.power = 1.f;
	//Definisco il colore che verrà assegnato allo schermo
	glClearColor(0.0, 0.0, 0.0, 0.0);

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

void drawScene(void)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Passo al Vertex Shader il puntatore alla matrice Projection, che sarà associata alla variabile Uniform mat4 Projection
	//all'interno del Vertex shader. Uso l'identificatio MatrixProj
	mat4 Projection = perspective(PerspectiveSetup.fovY, PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);
	mat4 View = lookAt(vec3(objectsP[0].translation + cameraOffset), vec3(objectsP[0].translation + cameraTargetOffset), vec3(0.0, 1.0, 0.0));

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
	glUniform3f(light_unif.light_position_pointer, light.position.x, light.position.y, light.position.z);
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	for (auto& obj : objectsP)
	{
		//rendo attivo il VAO dell'oggetto
		glUniform1i(lscelta, obj.material.shader);
		glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
		glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glBindVertexArray(obj.mesh.vertexArrayObjID);
		//Passaggio allo shader del Materiale Sfera
		glUniform3fv(light_unif.material_ambient, 1, value_ptr(obj.material.ambient));
		glUniform3fv(light_unif.material_diffuse, 1, value_ptr(obj.material.diffuse));
		glUniform3fv(light_unif.material_specular, 1, value_ptr(obj.material.specular));
		glUniform1f(light_unif.material_shininess, obj.material.shininess);

		glUniform3fv(lcenter, 1, value_ptr(obj.translation));

		//definisco le matrici di Modellazione
		Model = mat4(1.0);
		float wavePeriod = 25;
		float heightOffset = obj.floating ? 2 * sin(obj.translation.x * wavePeriod + currentTime / 1000) * sin(obj.translation.z * wavePeriod + currentTime / 1000) : 0;
		Model = translate(Model, obj.translation + vec3(0, heightOffset, 0));
		Model = rotate(Model, obj.rotation, vec3(0, 1, 0));
		Model = scale(Model, obj.scale);

		//Passo al Vertex Shader il puntatore alla matrice Model, che sarà associata alla variabile Uniform mat4 Projection
		//all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		//Disegno

		if (obj.mesh.isIndexed)
			glDrawElements(GL_TRIANGLES, obj.mesh.indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, obj.mesh.vertices.size());

		glBindVertexArray(0);
	}

	glutSwapBuffers();
}

void update(int a)
{
	currentTime += timestep;
	glUniform1f(loc_time, currentTime / 1000);
	glutTimerFunc(timestep, update, 0);

	speed.x -= sign(speed.x) * movementDrag * floatTimestep;
	speed.z -= sign(speed.z) * movementDrag * floatTimestep;

	if (length(speed) > maxSpeed) {
		speed = normalize(speed) * maxSpeed;
	}

	speed.x += float(inputX) * movementForce * floatTimestep;
	speed.z += float(inputY) * movementForce * floatTimestep;

	objectsP[0].translation += speed * floatTimestep;

	if (!(inputX == 0 && inputY == 0)) {
		targetDirection = normalize(vec2(inputX, inputY));
	}

	boatDirection = .8f * boatDirection + .2f * targetDirection;

	if (boatDirection.y > 0) {
		objectsP[0].rotation = -acosf(boatDirection.x) - pi<float>() / 2;
	}
	else {
		objectsP[0].rotation = acosf(boatDirection.x) - pi<float>() / 2;
	}

	glutPostRedisplay();
}

void onKeyPress(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		inputX -= 1;
		break;

	case 'd':
		inputX += 1;
		break;

	case 's':
		inputY += 1;
		break;

	case 'w':
		inputY -= 1;
		break;

	default:
		break;
	}
}

void onKeyRelease(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		inputX += 1;
		break;

	case 'd':
		inputX -= 1;
		break;

	case 's':
		inputY -= 1;
		break;

	case 'w':
		inputY += 1;
		break;

	default:
		break;
	}
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
	glutIgnoreKeyRepeat(GL_TRUE);
	glutKeyboardFunc(onKeyPress);
	glutKeyboardUpFunc(onKeyRelease);
	glutTimerFunc(timestep, update, 0);
	glewExperimental = GL_TRUE;
	glewInit();

	INIT_VAO();
	MatrixProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	MatView = glGetUniformLocation(programId, "View");

	MatModelS = glGetUniformLocation(programId1, "Model");
	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	MatViewS = glGetUniformLocation(programId1, "View");

	la = glGetUniformLocation(programId, "AmbientProduct");
	ld = glGetUniformLocation(programId, "DiffuseProduct");
	ls = glGetUniformLocation(programId, "SpecularProduct");
	lsh = glGetUniformLocation(programId, "Shininess");
	leye = glGetUniformLocation(programId, "eyePosition");
	lscelta = glGetUniformLocation(programId, "sceltaS");
	loc_time = glGetUniformLocation(programId, "time");
	idTex = glGetUniformLocation(programId, "tex0");
	lcenter = glGetUniformLocation(programId, "center");

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
