//
//  Mesh.h
//  Paanipuri
//
//  Bradley Crusco, Sanchit Garg, Debanshu Singh
//  Copyright (c) 2015 Crusco, Gar, Singh. All rights reserved.
//

#ifndef __paani__Mesh__
#define __paani__Mesh__

#include <iostream>
#include <vector>
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <glm/glm/glm.hpp>
#include <assimp/Scene.h>

//Reference : http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html

struct Vertex {
	glm::vec3 vertCoord;
	glm::vec3 normal;

	Vertex();
	Vertex(glm::vec3 v, glm::vec3 n) {
		vertCoord = v;
		normal = n;
	}
};


class Mesh {
public:
	Mesh() {}

	~Mesh() {}

	void LoadMesh(const std::string& Filename);

	void Render();

	int getNumVertices(int index);
	std::vector<glm::vec3> getTriangles(int index);
	std::vector<glm::vec3> getNormals(int index);

private:
	void InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int Index, const aiMesh* paiMesh);
	void Clear();

	struct MeshEntry {
		MeshEntry();

		~MeshEntry();

		void Init(const std::vector<Vertex>& Vertices,
			const std::vector<unsigned int>& Indices);

		GLuint VB;
		GLuint IB;
		unsigned int NumIndices;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	std::vector<MeshEntry> m_Entries;
};

#endif /* defined(__paani__Mesh__) */
