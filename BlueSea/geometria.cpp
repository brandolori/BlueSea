#include "Strutture.h"
#include "Lib.h"
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Strutture.h"
using namespace glm;
using namespace std;
double  random_range(double min, double max) {
	return min + static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / (max - min)));
}

void crea_sfera(vec3 centro, vec3 raggio, MeshP& sfera)
{
	int Stacks = 30;  //numero di suddivisioni sull'asse y
	int Slices = 30;  // numero di suddivisioni sull'asse x
	float s, t;
	Point coordinate;

	//Calc The Vertices

	//Stacks= suddivisioni lungo l'asse y
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * pi <float>();

		// Loop Through Slices suddivisioni lungo l'asse x
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (pi <float>() * 2);

			// Calc The Vertex Positions
			float x = centro.x + raggio.x * (cosf(theta) * sinf(phi));
			float y = centro.y + raggio.y * cosf(phi);
			float z = centro.z + raggio.z * sinf(theta) * sinf(phi);

			vec3 val = { x,y,z};
			// Push Back Vertex Data
			sfera.vertices.push_back(val);
			sfera.normals.push_back(normalize(vec3(x, y, z)));
			s=U;
			t=V;
			sfera.texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		sfera.indici.push_back(i);
		sfera.indici.push_back(i + Slices + 1);
		sfera.indici.push_back(i + Slices);


		sfera.indici.push_back(i + Slices + 1);
		sfera.indici.push_back(i);
		sfera.indici.push_back(i + 1);
	}


}


void crea_toro(MeshP& toro)
{
	int Stacks = 30;  //numero di suddivisioni sull'asse x
	int Slices = 30;  // numero di suddivisioni sull'asse y
	float R = 1, r = 0.5;
	float s, t;
	Point coordinate;

	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>() * 2;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = (R + r * cosf(phi)) * cosf(theta);
			float y = r * sinf(phi);
			float z = (R + r * cosf(phi)) * sinf(theta);


			// Push Back Vertex Data
			toro.vertices.push_back(vec3(x,y,z));
			toro.normals.push_back(normalize(vec3(sin(phi) * cos(theta), cos(phi), sin(theta) * sin(phi))));
			s = U;
			t = V;
			toro.texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		toro.indici.push_back(i);
		toro.indici.push_back(i + Slices + 1);
		toro.indici.push_back(i + Slices);


		toro.indici.push_back(i + Slices + 1);
		toro.indici.push_back(i);
		toro.indici.push_back(i + 1);
	}



}

void crea_cono(MeshP& cono)
{
	int Stacks = 30;  //numero di suddivisioni sull'asse x
	int Slices = 30;  // numero di suddivisioni sull'asse y
	float s, t;
	Point coordinate;

	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float h = V;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = h * cosf(theta);
			float y = h;
			float z = h * sinf(theta);


			// Push Back Vertex Data
			cono.vertices.push_back(vec3(x,y,z));
			cono.normals.push_back(normalize(vec3(cos(theta) / sqrt(2.0f), -1 / sqrt(2.0f), sin(theta) / sqrt(2.0f))));

			s = U;
			t = V;
			cono.texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		cono.indici.push_back(i);
		cono.indici.push_back(i + Slices + 1);
		cono.indici.push_back(i + Slices);


		cono.indici.push_back(i + Slices + 1);
		cono.indici.push_back(i);
		cono.indici.push_back(i + 1);
	}



}
void crea_cilindro(MeshP& cilindro)
{
	int Stacks = 30;  //numero di suddivisioni sull'asse x
	int Slices = 30;  // numero di suddivisioni sull'asse y

	Point coordinate;
	float s, t;
	//Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float h = V;

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = cosf(theta);
			float y = h;
			float z = sinf(theta);


			// Push Back Vertex Data
			cilindro.vertices.push_back(vec3(x,y,z));
			cilindro.normals.push_back(normalize(vec3(cos(theta), 0, sin(theta))));
			s = U;
			t = V;
			cilindro.texCoords.push_back(vec2(s, t));
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		cilindro.indici.push_back(i);
		cilindro.indici.push_back(i + Slices + 1);
		cilindro.indici.push_back(i + Slices);


		cilindro.indici.push_back(i + Slices + 1);
		cilindro.indici.push_back(i);
		cilindro.indici.push_back(i + 1);
	}



}

void crea_piano_suddiviso(MeshP& piano_suddiviso)
{

	int N = 256;

	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			piano_suddiviso.vertices.push_back(vec3((float)i / N, 0.0f, (float)j / N));
			piano_suddiviso.normals.push_back(vec3(0.0, 1.0, 0.0));
			piano_suddiviso.texCoords.push_back(vec2((float)j / N, (float)i / N));
	    }
	}
	int cont = -1;


	for (i = 0; i <= pow(N, 2) - (N + 1); i++) {


		j = i % (N);

		if (j != N - 1)
		{
			piano_suddiviso.indici.push_back(i);
			piano_suddiviso.indici.push_back(i + 1);
			piano_suddiviso.indici.push_back(i + N + 1);

			piano_suddiviso.indici.push_back(i + N + 1);
			piano_suddiviso.indici.push_back(i);
			piano_suddiviso.indici.push_back(i + N);

		}

	}

 }

 