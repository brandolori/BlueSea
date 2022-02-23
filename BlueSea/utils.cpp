#include "Strutture.h"

void crea_VAO_obj(MeshP* mesh)
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

	mesh->isIndexed = false;
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

	mesh->isIndexed = true;
}

