#include "Lib.h"
#include "Strutture.h"
double  random_range(double min, double max);
void crea_cono(MeshP& cono);
void crea_cilindro(MeshP& cilindro);
void crea_toro(MeshP& toro);
void crea_sfera(vec3 centro, vec3 raggio, MeshP& sfera);
void crea_piano_suddiviso(int resolution, MeshP& piano_suddiviso);
void crea_cubo(MeshP& mesh);
void crea_piramide(vector<Point>& vertices, vector <GLuint>& index_vertices);
void crea_piano(MeshP& piano);

