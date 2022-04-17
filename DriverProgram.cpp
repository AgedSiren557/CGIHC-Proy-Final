 /*
Semestre 2022-2
Práctica 5: Carga de Modelos
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture powerTexture;

//opengl en texturas en carpeta texturas
// en otros
//modelos a cargar
Model arbol1;
Model arbol2;
Model land;
Model pochita;
Model denji;
Model avion;
Model slide;


Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;


// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//requerido para la iluminacion y texturas
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};


	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,
	};
	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	
	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);
	// fin create objects

}

void CreatePower() {
	unsigned int torso_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat torso_vertices[] = {
		//x		y		z		S			T			NX		NY		NZ
		//front
		0.1f,	0.4f,	0.1f,	0.5449f,	0.8164f,		0.0f,	0.0f,	1.0f,	//0
		0.4f,	0.4f,	0.1f,	0.4014f,	0.8164f,		0.0f,	0.0f,	1.0f,	//1
		0.4f,	0.7f,	0.1f,	0.4014f,	0.8916f,		0.0f,	0.0f,	1.0f,	//2
		0.1f,	0.7f,	0.1f,	0.5449f,	0.8916f,		0.0f,	0.0f,	1.0f,	//3
		// right
		//x		y		z		S		T
		0.4f,	0.4f,	0.1f,	0.1953f,	0.1514f,		1.0f,	0.0f,	0.0f,	//4	
		0.4f,	0.4f,	0.2f,	0.1113f,	0.1514f,		1.0f,	0.0f,	0.0f,	//5
		0.4f,	0.7f,	0.2f,	0.1113f,	0.2275f,		1.0f,	0.0f,	0.0f,	//6
		0.4f,	0.7f,	0.1f,	0.1953f,	0.2275f,		1.0f,	0.0f,	0.0f,	//7
		// back
		0.1f,	0.4f,	0.2f,	0.0908f,	0.8173f,		0.0f,	0.0f,	-1.0f,	//8
		0.4f,	0.4f,	0.2f,	0.2383f,	0.8173f,		0.0f,	0.0f,	-1.0f,	//9
		0.4f,	0.7f,	0.2f,	0.2383f,	0.8926f,		0.0f,	0.0f,	-1.0f,	//10
		0.1f,	0.7f,	0.2f,	0.0908f,	0.8926f,		0.0f,	0.0f,	-1.0f,	//11

		// left
		//x		y		z		S		T
		0.1f,	0.4f,	0.1f,	0.4316f,  0.1602f,			-1.0f,	0.0f,	0.0f,	//12
		0.1f,	0.4f,	0.2f,	0.5138f,	0.1602f,		-1.0f,	0.0f,	0.0f,	//13
		0.1f,	0.7f,	0.2f,	0.5137f,	0.2363f,		-1.0f,	0.0f,	0.0f,	//14
		0.1f,	0.7f,	0.1f,	0.4316f,	0.2363f,		-1.0f,	0.0f,	0.0f,	//15

		// bottom
		//x		y		z		S		T
		0.1f,	0.4f,	0.1f,	0.1035f,	0.5205f,		0.0f,	-1.0f,	0.0f,	//16
		0.4f,	0.4f,	0.1f,	0.1846f,	0.5205f,		0.0f,	-1.0f,	0.0f,	//17
		0.4f,	0.4f,	0.2f,	0.1846f,	0.5459f,		0.0f,	-1.0f,	0.0f,	//18
		0.1f,	0.4f,	0.2f,	0.1035f,	0.5459f,		0.0f,	-1.0f,	0.0f,	//19

		//UP
		//x		y		z		S		T
		0.1f,	0.7f,	0.1f,	0.1035f,	0.5205f,		0.0f,	1.0f,	0.0f,	//20
		0.4f,	0.7f,	0.1f,	0.1846f,	0.5205f,		0.0f,	1.0f,	0.0f,	//21
		0.4f,	0.7f,	0.2f,	0.1846f,	0.5459f,		0.0f,	1.0f,	0.0f,	//22
		0.1f,	0.7f,	0.2f,	0.1035f,	0.5459f,		0.0f,	1.0f,	0.0f,	//23

	};

	unsigned int brazoi_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat brazoi_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		//front
		0.0f,	0.3f,	0.1f,	0.5927f,	0.7949f,		0.0f,	0.0f,	1.0f,	//0
		0.1f,	0.3f,	0.1f,	0.5458f,	0.7949f,		0.0f,	0.0f,	1.0f,	//1
		0.1f,	0.7f,	0.1f,	0.5458f,	0.8916f,		0.0f,	0.0f,	1.0f,	//2
		0.0f,	0.7f,	0.1f,	0.5927f,	0.8916f,		0.0f,	0.0f,	1.0f,	//3
		// right
		//x		y		z		S		T
		0.1f,	0.3f,	0.1f,	0.4355f,	0.4580f,		1.0f,	0.0f,	0.0f,	//4
		0.1f,	0.3f,	0.2f,	0.5186f,	0.4580f,		1.0f,	0.0f,	0.0f,	//5
		0.1f,	0.7f,	0.2f,	0.5186f,	0.5547f,		1.0f,	0.0f,	0.0f,	//6
		0.1f,	0.7f,	0.1f,	0.4355f,	0.5547f,		1.0f,	0.0f,	0.0f,	//7
		// back
		0.0f,	0.3f,	0.2f,	0.041f, 0.7949f,		0.0f,	0.0f,	1.0f,	
		0.1f,	0.3f,	0.2f,	0.090f,	0.7949f,		0.0f,	0.0f,	1.0f,	
		0.1f,	0.7f,	0.2f,	0.090f,	0.8926f,		0.0f,	0.0f,	1.0f,	
		0.0f,	0.7f,	0.2f,	0.041f,	0.8926f,		0.0f,	0.0f,	1.0f,	

		// left
		0.0f,	0.3f,	0.1f,	0.4355f,	0.4580f,		1.0f,	0.0f,	0.0f,	
		0.0f,	0.3f,	0.2f,	0.5186f,	0.4580f,		1.0f,	0.0f,	0.0f,	
		0.0f,	0.7f,	0.2f,	0.5186f,	0.5547f,		1.0f,	0.0f,	0.0f,	
		0.0f,	0.7f,	0.1f,	0.4355f,	0.5547f,		1.0f,	0.0f,	0.0f,	

		// bottom
		0.0f,	0.3f,	0.1f,	0.6602f,	0.46f,			0.0f,	0.0f,	1.0f,	
		0.1f,	0.3f,	0.1f,	0.7080f,	0.46f,			0.0f,	0.0f,	1.0f,	
		0.1f,	0.3f,	0.2f,	0.7080f,	0.5439f,		0.0f,	0.0f,	1.0f,
		0.0f,	0.3f,	0.2f,	0.6602f,	0.5439f,		0.0f,	0.0f,	1.0f,	

		//UP
		//x		y		z		S		T
		0.0f,	0.7f,	0.1f,	0.9316f,	0.8135f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.7f,	0.1f,	0.8877f,	0.8135f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.7f,	0.2f,	0.8877f,	0.8936f,		0.0f,	0.0f,	1.0f,
		0.0f,	0.7f,	0.2f,	0.9316f,	0.8936f,		0.0f,	0.0f,	1.0f,

	};

	unsigned int brazod_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat brazod_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		//front
		0.4f,	0.3f,	0.1f,	0.4014f,	0.7949f,		0.0f,	0.0f,	1.0f,	//0
		0.5f,	0.3f,	0.1f,	0.3535f,	0.7949f,		0.0f,	0.0f,	1.0f,	//1
		0.5f,	0.7f,	0.1f,	0.3535f,	0.8916f,		0.0f,	0.0f,	1.0f,	//2
		0.4f,	0.7f,	0.1f,	0.4014f,	0.8916f,		0.0f,	0.0f,	1.0f,	//3
		// right
		//x		y		z		S		T
		0.5f,	0.3f,	0.1f,	0.1025f,	0.4502f,		1.0f,	0.0f,	0.0f,	//4
		0.5f,	0.3f,	0.2f,	0.1846f,	0.4502f,		1.0f,	0.0f,	0.0f,	//5
		0.5f,	0.7f,	0.2f,	0.1846f,	0.5469f,		1.0f,	0.0f,	0.0f,	//6
		0.5f,	0.7f,	0.1f,	0.1025f,	0.5469f,		1.0f,	0.0f,	0.0f,	//7
		// back
		0.4f,	0.3f,	0.2f,	0.2383f,	0.7949f,		0.0f,	0.0f,	1.0f,
		0.5f,	0.3f,	0.2f,	0.2832f,	0.7949f,		0.0f,	0.0f,	1.0f,
		0.5f,	0.7f,	0.2f,	0.2832f,	0.8926f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.2f,	0.2383f,	0.8926f,		0.0f,	0.0f,	1.0f,

		// left
		0.4f,	0.3f,	0.1f,	0.1025f,	0.4502f,		1.0f,	0.0f,	0.0f,
		0.4f,	0.3f,	0.2f,	0.1846f,	0.4502f,		1.0f,	0.0f,	0.0f,
		0.4f,	0.7f,	0.2f,	0.1846f,	0.5469f,		1.0f,	0.0f,	0.0f,
		0.4f,	0.7f,	0.1f,	0.1025f,	0.5469f,		1.0f,	0.0f,	0.0f,

		// bottom
		0.4f,	0.3f,	0.1f,	0.6602f,	0.46f,		0.0f,	0.0f,	1.0f,	//mal
		0.5f,	0.3f,	0.1f,	0.7080f,	0.46f,		0.0f,	0.0f,	1.0f,
		0.5f,	0.3f,	0.2f,	0.7080f,	0.5439f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.3f,	0.2f,	0.6602f,	0.5439f,		0.0f,	0.0f,	1.0f,

		//UP
		//x		y		z		S		T
		0.4f,	0.7f,	0.1f,	0.7412f,	0.8125f,		0.0f,	0.0f,	1.0f,
		0.5f,	0.7f,	0.1f,	0.6914f,	0.8125f,		0.0f,	0.0f,	1.0f,
		0.5f,	0.7f,	0.2f,	0.6914f,	0.8945f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.2f,	0.7412f,	0.8945f,		0.0f,	0.0f,	1.0f,

	};

	unsigned int piernad_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat piernad_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		//front
		0.25f,	0.0f,	0.1f,	0.4736f,	0.7178f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.0f,	0.1f,	0.4014f,	0.7178f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.4f,	0.1f,	0.4014f,	0.8164f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.1f,	0.4736f,	0.8164f,		0.0f,	0.0f,	1.0f,
		// right
		//x		y		z		S		T
		0.4f,	0.0f,	0.1f,	0.1953f,	0.0527f,		1.0f,	0.0f,	0.0f,	
		0.4f,	0.0f,	0.2f,	0.1113f,	0.0527f,		1.0f,	0.0f,	0.0f,	
		0.4f,	0.4f,	0.2f,	0.1113f,	0.1514f,		1.0f,	0.0f,	0.0f,	
		0.4f,	0.4f,	0.1f,	0.1953f,	0.1514f,		1.0f,	0.0f,	0.0f,	
		// back
		0.25f,	0.0f,	0.2f,	0.1646f,	0.7188f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.0f,	0.2f,	0.2383f,	0.7188f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.4f,	0.2f,	0.2383f,	0.8174f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.2f,	0.1646f,	0.8174f,		0.0f,	0.0f,	1.0f,

		// left
		0.25f,	0.0f,	0.1f,	0.8496f,	0.0537f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.0f,	0.2f,	0.9102f,	0.0537f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.4f,	0.2f,	0.9102f,	0.3018f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.4f,	0.1f,	0.8496f,	0.3018f,		1.0f,	0.0f,	0.0f,

		// bottom
		0.25f,	0.0f,	0.1f,	0.71f,		0.4609f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.0f,	0.1f,	0.8564f,	0.4609f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.0f,	0.2f,	0.8564f,	0.5439f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.0f,	0.2f,	0.71f,		0.5439f,		0.0f,	0.0f,	1.0f,

		//UP
		//x		y		z		S		T
		0.25f,	0.4f,	0.1f,	0.33f,  0.5f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.4f,	0.1f,	0.66f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.4f,	0.2f,	0.66f,	0.75f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.2f,	0.33f,	0.75f,		0.0f,	0.0f,	1.0f,

	};

	unsigned int piernai_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat piernai_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		//front
		0.1f,	0.0f,	0.1f,	0.5458f,	0.7178f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.0f,	0.1f,	0.4736f,	0.7178f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.1f,	0.4736f,	0.8164f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.4f,	0.1f,	0.5458f,	0.8164f,		0.0f,	0.0f,	1.0f,
		// right
		//x		y		z		S		T
		0.25f,	0.0f,	0.1f,	0.33f,  0.5f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.0f,	0.2f,	0.66f,	0.5f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.4f,	0.2f,	0.66f,	0.75f,		1.0f,	0.0f,	0.0f,
		0.25f,	0.4f,	0.1f,	0.33f,	0.75f,		1.0f,	0.0f,	0.0f,
		// back
		0.1f,	0.0f,	0.2f,	0.0908f,	0.7188f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.0f,	0.2f,	0.1646f,	0.7188f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.2f,	0.1646f,	0.8174f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.4f,	0.2f,	0.0908f,	0.8174f,		0.0f,	0.0f,	1.0f,

		// left
		0.1f,	0.0f,	0.1f,	0.4316f,	0.06347f,		1.0f,	0.0f,	0.0f,
		0.1f,	0.0f,	0.2f,	0.5137f,	0.06347f,		1.0f,	0.0f,	0.0f,
		0.1f,	0.4f,	0.2f,	0.5137f,	0.1602f,		1.0f,	0.0f,	0.0f,
		0.1f,	0.4f,	0.1f,	0.4316f,	0.1602f,		1.0f,	0.0f,	0.0f,

		// bottom
		0.1f,	0.0f,	0.1f,	0.71f,		0.4609f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.0f,	0.1f,	0.8564f,	0.4609f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.0f,	0.2f,	0.8564f,	0.5439f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.0f,	0.2f,	0.71f,		0.5439f,		0.0f,	0.0f,	1.0f,

		//UP
		//x		y		z		S		T
		0.1f,	0.4f,	0.1f,	0.33f,  0.5f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.1f,	0.66f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.25f,	0.4f,	0.2f,	0.66f,	0.75f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.4f,	0.2f,	0.33f,	0.75f,		0.0f,	0.0f,	1.0f,

	};

	unsigned int cabeza_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		9,8,11,
		11,10,9,
		// left
		13,12,15,
		15,14,13,

		// bottom	16,17,18,19
		16, 17, 18,
		18,19,16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat cabeza_vertices[] = {
		//x		y		z		S		T			NX		NY		NZ
		//front
		0.1f,	0.7f,	0.0f,	0.5449f,	0.8916f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.0f,	0.4013f,	0.8916f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.0f,	0.4013f,	0.9658f,		0.0f,	0.0f,	1.0f,
		0.1f,	1.0f,	0.0f,	0.5449f,	0.9658f,		0.0f,	0.0f,	1.0f,
		// right
		//x		y		z		S		T
		0.4f,	0.7f,	0.0f,	0.0273f,	0.5479f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.3f,	0.2695f,	0.5479f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.3f,	0.2695f,	0.6221f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.0f,	0.0273f,	0.6221f,		0.0f,	0.0f,	1.0f,
		// back
		0.1f,	0.7f,	0.3f,	0.0908f,	0.8926f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.3f,	0.2373f,	0.8926f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.3f,	0.2373f,	0.9668f,		0.0f,	0.0f,	1.0f,
		0.1f,	1.0f,	0.3f,	0.0908f,	0.9668f,		0.0f,	0.0f,	1.0f,

		// left
		0.1f,	0.7f,	0.0f,	0.3525f,	0.5547f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.7f,	0.3f,	0.5947f,	0.5547f,		0.0f,	0.0f,	1.0f,
		0.1f,	1.0f,	0.3f,	0.5947f,	0.6289f,		0.0f,	0.0f,	1.0f,
		0.1f,	1.0f,	0.0f,	0.3525f,	0.6289f,		0.0f,	0.0f,	1.0f,

		// bottom
		0.1f,	0.7f,	0.0f,	0.6680f,	0.3016f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.0f,	0.6680f,	0.1152f,		0.0f,	0.0f,	1.0f,
		0.4f,	0.7f,	0.3f,	0.8477f,	0.1152f,		0.0f,	0.0f,	1.0f,
		0.1f,	0.7f,	0.3f,	0.8477f,	0.3016f,		0.0f,	0.0f,	1.0f,

		//UP
		//x		y		z		S		T
		0.1f,	1.0f,	0.0f,	0.8857f,	0.7295f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.0f,	0.7412f,	0.7295f,		0.0f,	0.0f,	1.0f,
		0.4f,	1.0f,	0.3f,	0.7412f,	0.9688f,		0.0f,	0.0f,	1.0f,
		0.1f,	1.0f,	0.3f,	0.8857f,	0.9688f,		0.0f,	0.0f,	1.0f,

	};

	Mesh* powerTorso = new Mesh();
	powerTorso->CreateMesh(torso_vertices, torso_indices, 192, 36);
	meshList.push_back(powerTorso);

	Mesh* powerBrazoI = new Mesh();
	powerBrazoI->CreateMesh(brazoi_vertices, brazoi_indices, 192, 36);
	meshList.push_back(powerBrazoI);

	Mesh* powerBrazoD = new Mesh();
	powerBrazoD->CreateMesh(brazod_vertices, brazod_indices, 192, 36);
	meshList.push_back(powerBrazoD);

	Mesh* powerPiernaD = new Mesh();
	powerPiernaD->CreateMesh(piernad_vertices, piernad_indices, 192, 36);
	meshList.push_back(powerPiernaD);

	Mesh* powerPiernaI = new Mesh();
	powerPiernaI->CreateMesh(piernai_vertices, piernai_indices, 192, 36);
	meshList.push_back(powerPiernaI);

	Mesh* powerCabeza = new Mesh();
	powerCabeza->CreateMesh(cabeza_vertices, cabeza_indices, 192, 36);
	meshList.push_back(powerCabeza);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

void RenderPower(glm::mat4 model, GLuint uniformModel, GLuint uniformSpecularIntensity, GLuint uniformShininess) {
	
	//torso
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	powerTexture.UseTexture();
	meshList[3]->RenderMesh();
	//brazo izquierdo
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[4]->RenderMesh();
	//brazo derecho
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[5]->RenderMesh();
	//pierna derecha
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[6]->RenderMesh();
	//pierna izquierda
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[7]->RenderMesh();
	//cabeza
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	meshList[8]->RenderMesh();
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreatePower();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);


	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	powerTexture = Texture("Textures/PowerTexture1024.tga");
	powerTexture.LoadTextureA();

	/*
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_obj.obj");
	*/
	arbol1 = Model();
	arbol1.LoadModel("Models/arbol1.obj");
	arbol2 = Model();
	arbol2.LoadModel("Models/tree1.obj");
	land = Model();
	land.LoadModel("Models/floor.obj");
	pochita = Model();
	pochita.LoadModel("Models/pochita.obj");
	denji = Model();
	denji.LoadModel("Models/denji.obj");
	avion = Model();
	avion.LoadModel("Models/legoPlane.obj");
	slide = Model();
	slide.LoadModel("Models/slide.obj");

	std::vector<std::string> skyboxFaces;	//cubo de el paisaje de fondo cubemap, se desabilita el buffer de profundidad y colooca skybox en el inifnito
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		//piso
		model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(31.0f, 0.0f, 31.0f));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		land.RenderModel();

		//arbol 1 
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(8.0, 8.0f, 8.0f));
		model = glm::translate(model, glm::vec3(-11.0f, 0.0f, 13.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		arbol1.RenderModel();

		//arbol 1 - atras
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(4.0, 8.0f, 4.0f));
		model = glm::translate(model, glm::vec3(-22.0f, 0.0f, 49.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		arbol1.RenderModel();

		//arbol 2 - Pino 1
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(2.0, 2.0f, 2.0f));
		model = glm::translate(model, glm::vec3(-21.0f, 0.0f, 46.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		arbol2.RenderModel();

		//pochita
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.5, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-300.0f, 2.4f, 250.0f));
		model = glm::rotate(model, -180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		pochita.RenderModel();

		//slide
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(3.0, 3.0f, 3.0f));
		model = glm::translate(model, glm::vec3(-50.0f, 0.0f, 45.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		slide.RenderModel();

		//denji
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(0.5, 0.5f, 0.5f));
		model = glm::translate(model, glm::vec3(-150.0f, 0.0f, 125.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		denji.RenderModel();

		//avion
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(-30.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		avion.RenderModel();

		/*****************Power**************/
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 30.0f));
		RenderPower(model, uniformModel, uniformSpecularIntensity, uniformShininess);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
