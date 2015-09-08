//
//  Main.cpp
//  Paanipuri
//
//  Bradley Crusco, Sanchit Garg, Debanshu Singh
//  Copyright (c) 2015 Crusco, Gar, Singh. All rights reserved.
//

#include "Scene.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include "SOIL.h"
#include "Shader.h"

using namespace glm;
Scene* scene;

GLFWwindow* gWindow;
GLuint gVAO = 0;
GLuint gBufPos = 0;
GLuint gBufCol = 0;
GLuint gBufSiz = 0;

GLuint _textureId; //The id of the texture
GLuint shaderProgram;

GLuint locationPos;
GLuint locationCol;
GLuint locationNor;
GLuint locationSiz;

GLuint unifProj;
GLuint unifView;
GLuint unifModel;
GLuint unifModelInvTr;
GLuint unifLightPos;
GLuint unifLightColor;
GLuint unifCamPos;

/**************/
// Floor setup
/**************/
GLuint planeVAO, planeVBO;
GLboolean blinn = true;
GLuint floorTexture;

/**************/
// Camera setup
/**************/
glm::vec3 camEye = glm::vec3(0, 7, 150);
glm::vec3 camDir = glm::vec3(0, -5, 0);
glm::vec3 camUp = glm::vec3(0, -1, 0);
GLfloat yaw1 = -90.0f;
GLfloat pitch1 = 0.0f;

/**************/
// Mouse setup
/**************/
GLfloat lastX = SCREEN_SIZE.x / 2.0;
GLfloat lastY = SCREEN_SIZE.y / 2.0;
bool firstMouse = true;

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

void init(int argc, char* argv[]);
void display();
void displayParticles();
void displayPlane(const Shader& shader);

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	GLfloat sensitivity = 0.5f;

	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(gWindow, GL_TRUE);
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		scene->addParticlesToScene(1);
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		scene->addParticlesToScene(2);
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		scene->addBallToScene();
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		scene->addCubeToScene();
	else if (key == GLFW_KEY_5)
		scene->pourFluid(1);
	else if (key == GLFW_KEY_6)
		scene->pourFluid(2);
	else if (key == GLFW_KEY_W)
		camEye += sensitivity*camDir;
	else if (key == GLFW_KEY_S)
		camEye -= sensitivity*camDir;
	else if (key == GLFW_KEY_A)
		camEye += sensitivity*glm::normalize(glm::cross(camDir, camUp));
	else if (key == GLFW_KEY_D)
		camEye -= sensitivity*glm::normalize(glm::cross(camDir, camUp));

}

static void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	//source - http://www.learnopengl.com/

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat deltaX = xpos - lastX;
	GLfloat deltaY = lastY - ypos;

	lastX = xpos; lastY = ypos;

	GLfloat sensitivity = 0.025f;
	deltaX *= sensitivity;
	deltaY *= sensitivity;

	yaw1 += deltaX;
	pitch1 += deltaY;

	if (pitch1 > 89.0f)
		pitch1 = 89.0f;
	if (pitch1 < -89.0f)
		pitch1 = -89.0f;

	glm::vec3 dir;
	dir.x = cos(glm::radians(yaw1)) * cos(glm::radians(pitch1));
	dir.y = sin(glm::radians(pitch1));
	dir.z = sin(glm::radians(yaw1)) * cos(glm::radians(pitch1));
	camDir = glm::normalize(dir);
}


GLuint loadTexture(GLchar* imagepath) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	int width, height;
	unsigned char* image = SOIL_load_image(imagepath, &width, &height, 0, SOIL_LOAD_RGB);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureId;
}

std::string textFileRead(const char *filename) {
	// http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
	std::ifstream in(filename, std::ios::in);
	if (!in) {
		std::cerr << "Error reading file" << std::endl;
		throw (errno);
	}
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void printShaderInfoLog(int shader) {
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_TRUE) {
		return;
	}
	std::cerr << "GLSL COMPILE ERROR" << std::endl;

	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0) {
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
		delete[] infoLog;
	}
	// Throwing here allows us to use the debugger to track down the error.
	throw;
}

void printGLErrorLog() {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error " << error << ": ";
		const char *e =
			error == GL_INVALID_OPERATION ? "GL_INVALID_OPERATION" :
			error == GL_INVALID_ENUM ? "GL_INVALID_ENUM" :
			error == GL_INVALID_VALUE ? "GL_INVALID_VALUE" :
			error == GL_INVALID_INDEX ? "GL_INVALID_INDEX" :
			"unknown";
		std::cerr << e << std::endl;
		// Throwing here allows us to use the debugger stack trace to track
		// down the error.
#ifndef __APPLE__
		// But don't do this on OS X. It might cause a premature crash.
		// http://lists.apple.com/archives/mac-opengl/2012/Jul/msg00038.html
		throw;
#endif
	}
}

void printLinkInfoLog(int prog) {
	GLint linked;
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if (linked == GL_TRUE) {
		return;
	}
	std::cerr << "GLSL LINK ERROR" << std::endl;

	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

	if (infoLogLen > 0) {
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
		std::cerr << "InfoLog:" << std::endl << infoLog << std::endl;
		delete[] infoLog;
	}
	// Throwing here allows us to use the debugger to track down the error.
	throw;
}

void initShader() {
	// Read in the shader program source files

	std::string vertSourceS = textFileRead("../Paanipuri_Windows/Shaders/vs_particle.glsl");
	std::string fragSourceS = textFileRead("../Paanipuri_Windows/Shaders/fs_particle.glsl");

	const char *vertSource = vertSourceS.c_str();
	const char *fragSource = fragSourceS.c_str();

	// Tell the GPU to create new shaders and a shader program
	GLuint shadVert = glCreateShader(GL_VERTEX_SHADER);
	GLuint shadFrag = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = glCreateProgram();

	// Load and compiler each shader program
	// Then check to make sure the shaders complied correctly

	// - Vertex shader
	glShaderSource(shadVert, 1, &vertSource, NULL);
	glCompileShader(shadVert);
	printShaderInfoLog(shadVert);
	// - Diffuse fragment shader
	glShaderSource(shadFrag, 1, &fragSource, NULL);
	glCompileShader(shadFrag);
	printShaderInfoLog(shadFrag);

	// Link the shader programs together from compiled bits
	glAttachShader(shaderProgram, shadVert);
	glAttachShader(shaderProgram, shadFrag);
	glLinkProgram(shaderProgram);
	printLinkInfoLog(shaderProgram);

	// Clean up the shaders now that they are linked
	glDetachShader(shaderProgram, shadVert);
	glDetachShader(shaderProgram, shadFrag);
	glDeleteShader(shadVert);
	glDeleteShader(shadFrag);

	// Find out what the GLSL locations are, since we can't pre-define these
	locationPos = glGetAttribLocation(shaderProgram, "vs_Position");
	//    locationNor    = glGetAttribLocation (shaderProgram, "vs_Normal");
	locationCol = glGetAttribLocation(shaderProgram, "vs_Color");
	locationSiz = glGetAttribLocation(shaderProgram, "vs_Size");

	unifProj = glGetUniformLocation(shaderProgram, "u_projection");
	unifView = glGetUniformLocation(shaderProgram, "u_view");
	unifModel = glGetUniformLocation(shaderProgram, "u_model");

	/**************/
	// Camera setup
	/**************/
	glUseProgram(shaderProgram);

	glm::mat4 view = glm::lookAt(camEye, camEye + camDir, camUp);
	glUniformMatrix4fv(unifView, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::perspective<float>(50.0, (float)SCREEN_SIZE.x / SCREEN_SIZE.y, 0.1f, 2000.0f);
	glUniformMatrix4fv(unifProj, 1, GL_FALSE, &projection[0][0]);

	glUseProgram(0);
}

void loadPlane() {
	/**************/
	// Floor setup
	/**************/

	// setup floor geom
	GLfloat planeVertices[] = {
		// Positions          // Normals         // Texture Coords
		32.0f, -10.0f, 32.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-32.0f, -10.0f, 32.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		-32.0f, -10.0f, -32.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		32.0f, -10.0f, 32.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-32.0f, -10.0f, -32.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		32.0f, -10.0f, -32.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// setup floor texture
	floorTexture = loadTexture("../Paanipuri_Windows/Textures/wood.png");
}

void displayPlane(Shader& shader) {
	shader.Use();

	glm::mat4 view = glm::lookAt(camEye, camEye + camDir, camUp);
	glm::mat4 projection = glm::perspective<float>(50.0, (float)SCREEN_SIZE.x / SCREEN_SIZE.y, 0.1f, 2000.0f);

	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//set light uniforms
	glUniform3fv(glGetUniformLocation(shader.Program, "lightPos"), 1, &lightPos[0]);
	glUniform3fv(glGetUniformLocation(shader.Program, "viewPos"), 1, &camEye[0]);
	glUniform1i(glGetUniformLocation(shader.Program, "blinn"), blinn);

	// Floor
	glBindVertexArray(planeVAO);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void loadParticles() {
	// Put the three verticies into the VBO

	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	glGenBuffers(1, &gBufPos);
	glBindBuffer(GL_ARRAY_BUFFER, gBufPos);
	glBufferData(GL_ARRAY_BUFFER, scene->particleSystem->particlePosData.size()*sizeof(glm::vec3), \
		&scene->particleSystem->particlePosData[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(locationPos);
	glVertexAttribPointer(locationPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gBufCol);
	glBindBuffer(GL_ARRAY_BUFFER, gBufCol);
	glBufferData(GL_ARRAY_BUFFER, scene->particleSystem->particleColData.size()*sizeof(glm::vec3), \
		&scene->particleSystem->particleColData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(locationCol);
	glVertexAttribPointer(locationCol, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gBufSiz);
	glBindBuffer(GL_ARRAY_BUFFER, gBufSiz);
	glBufferData(GL_ARRAY_BUFFER, scene->particleSystem->particleSizData.size()*sizeof(glm::vec3), \
		&scene->particleSystem->particleSizData[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(locationSiz);
	glVertexAttribPointer(locationSiz, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void displayParticles() {
	// Put the three verticies into the VBO
	std::vector<glm::vec3> particlePosData;
	std::vector<glm::vec3> particleColData;
	std::vector<glm::vec3> particleSizData;

	std::vector<Particle> particles = scene->particleSystem->getAllParticles();

	for (std::vector<Particle>::iterator it = particles.begin(); it < particles.end(); it++) {
		Particle particle = *it;
		glm::vec3 partPos = particle.getPosition();
		particlePosData.push_back(partPos);

		glm::vec3 partCol(0.0, 1.0, 1.0);
		glm::vec3 partSize(2.f);

		if (particle.getPhase() == 1) {
			partCol = glm::vec3(1.0, 1.0, 1.0);
		}
		else if (particle.getPhase() >= 2) {

			partSize = glm::vec3(5.f);
			if (particle.getPhase() % 4 == 0) { //sphere
				partCol = glm::vec3(1.0, 1.0, 0.0);
			}
			else if (particle.getPhase() % 2 == 0) { //sphere
				partCol = glm::vec3(1.0, 0.0, 0.0);
			}
			else if ((particle.getPhase() - 1) % 4 == 0) {  //cube
				partCol = glm::vec3(1.0, 0.54901960784, 0.0);
			}
			else {
				partCol = glm::vec3(0.49803921568, 1.0, 0.0);
			}
		}
		particleColData.push_back(partCol);
		particleSizData.push_back(partSize);
	}

	// Tell the GPU which shader program to use to draw things
	// bind the program (the shaders)
	glUseProgram(shaderProgram);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// set model matrix
	glm::mat4 model = glm::mat4();
	glUniformMatrix4fv(unifModel, 1, GL_FALSE, &model[0][0]);

	glm::mat4 view = glm::lookAt(camEye, camEye + camDir, camUp);
	glUniformMatrix4fv(unifView, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::perspective<float>(50.0, (float)SCREEN_SIZE.x / SCREEN_SIZE.y, 0.1f, 2000.0f);
	glUniformMatrix4fv(unifProj, 1, GL_FALSE, &projection[0][0]);

	glBindVertexArray(gVAO);

	//bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, gBufPos);
	glBufferData(GL_ARRAY_BUFFER, particlePosData.size()*sizeof(glm::vec3), &particlePosData[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(locationPos);
	glVertexAttribPointer(locationPos, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, gBufSiz);
	glBufferData(GL_ARRAY_BUFFER, particleSizData.size()*sizeof(glm::vec3), &particleSizData[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(locationSiz);
	glVertexAttribPointer(locationSiz, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, gBufCol);
	glBufferData(GL_ARRAY_BUFFER, particleColData.size()*sizeof(glm::vec3), &particleColData[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(locationCol);
	glVertexAttribPointer(locationCol, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	const GLuint numIndices = scene->numberOfParticles;
	glDrawArrays(GL_POINTS, 0, numIndices);

	// unbind the VAO
	glBindVertexArray(0);
	// unbind the program
	glUseProgram(0);
}


void initGLFW(int argc, char* argv[]) {
	if (!glfwInit())
		throw std::runtime_error("glfwInit failed");

	// open a window with GLFW
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "paanipuri", NULL, NULL);
	if (!gWindow)
		throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");
	glfwMakeContextCurrent(gWindow);

	glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
	if (glewInit() != GLEW_OK)
		throw std::runtime_error("glewInit failed");
	// print out some info about the graphics drivers
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	// make sure OpenGL version 3.2 API is available
	if (!GLEW_VERSION_3_2)
		throw std::runtime_error("OpenGL 3.2 API is not available.");

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE); // enable point size to be changed in vertex shader

	glfwSetKeyCallback(gWindow, keyCallback);
	glfwSetCursorPosCallback(gWindow, mouseCallback);
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

}

int main(int argc, char * argv[]) {

	initGLFW(argc, argv);

	initShader();

	scene = new Scene();
	scene->init();

	Shader floorShader("../Paanipuri_Windows/Shaders/vs_floor.glsl", "../Paanipuri_Windows/Shaders/fs_floor.glsl");
	loadPlane();

	loadParticles();

	// run while the window is open
	while (!glfwWindowShouldClose(gWindow)) {
		// process pending events
		glfwPollEvents();

		// Clear the colorbuffer
		glClearColor(0.f, 0.f, 0.f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw one frame
		displayPlane(floorShader);
		scene->update();
		displayParticles();

		glfwSwapBuffers(gWindow);
	}

	// clean up and exit
	glfwTerminate();

	return 0;
}