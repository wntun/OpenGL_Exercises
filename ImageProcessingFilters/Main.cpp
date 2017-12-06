//
//  CGIPExamples.cpp
//  OpenGLExamples
//
//  Created by KIM HYUNG SEOK on 2016. 4. 5..
//  Copyright © 2016년 KIM HYUNG SEOK. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include<algorithm>

#include <GL/glew.h>
#include <GL/glut.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

#define NOFILTER 0
#define NEGATE 1
#define GAUSSIAN 2
#define AVERAGE 3


GLuint myProgram;
int g_windowWidth, g_windowHeight;
GLuint VertexArrayID[1];
int filterChoice = 0;
GLuint filterLoc;


std::vector<float> g_aVtxPos;
std::vector<float> g_aSize;
float angle = 0;

GLuint VBOs[1];

float pointColor[3] = { 1.0, 0.0, 0.0 };

GLuint texID[1];

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		//fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> FragmentShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	}

	//Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.
	if (g_aVtxPos.size() > 0)
	{
		glBindVertexArray(VertexArrayID[0]);
	}
	glDrawArrays(GL_TRIANGLES, 0, 38);
	glutSwapBuffers();
	glutPostRedisplay();
}

void mousePoint(int button, int state, int x, int y)
{

}


void init()
{
	//initilize the glew and check the errors.

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}

	//select the background color

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);
}

void processMenuEvent(int fOption) {
	filterChoice = fOption;
	glUniform1i(filterLoc, filterChoice);
	glutPostRedisplay();

	/*switch (fOption) {
	case NEGATE:
		filterChoice = NEGATE;
		break;
	case GAUSSIAN:
		break;
	case AVERAGE:
		break;
	case NOFILTER:
		break;
	}*/
}

void createMenu() {
	int menu = glutCreateMenu(processMenuEvent);
	glutAddMenuEntry("Image Negative", NEGATE);
	glutAddMenuEntry("Gaussian filter", GAUSSIAN);
	glutAddMenuEntry("Average filter", AVERAGE);
	glutAddMenuEntry("None", NOFILTER);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv)
{
	Mat image;
	image = imread("cosplay.jpg", CV_LOAD_IMAGE_COLOR);

	if (image.empty()) {
		printf("Could not find an image");
		return -1;
	}
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//These two functions are used to define the position and size of the window.
	glutInitWindowPosition(200, 200);

	g_windowWidth = image.cols;
	g_windowHeight = image.rows;
	glutInitWindowSize(g_windowWidth, g_windowHeight);
	//This is used to define the name of the window.
	glutCreateWindow("Image Processing: Filters");

	//call initization function
	init();

	//3.
	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	glUseProgram(programID);

	myProgram = programID;

	GLfloat vtxBufferData[] = {
		-1,-1,-1,  0,0,// triangle 1 : begin
		-1,-1, 1,  0,0,
		-1, 1, 1,  1,0, // triangle 1 : end
		-1,-1,-1,  0,1,
		-1, 1, 1,  1,0,
		-1, 1,-1,  1,1,

		1, 1,-1,  1,0, // triangle 3 : begin
		-1,-1,-1, 0,1,
		-1, 1,-1, 0,0, // triangle 3 : end
		1, 1,-1,  1,0,
		1,-1,-1,  1,1,
		-1,-1,-1, 0,1,

		1,-1, 1,  1,0,
		-1,-1,-1, 0,1,
		1,-1,-1,  1,1,
		1,-1, 1,  1,0,
		-1,-1, 1, 0,0,
		-1,-1,-1, 0,1,

		-1, 1, 1, 0, 0,
		-1,-1, 1, 0, 1,
		1,-1, 1,  1, 1,
		1, 1, 1,  1, 0,
		-1, 1, 1, 0, 0,
		1,-1, 1,  1, 1,

		1, 1, 1,  1, 0,
		1,-1,-1,  0, 1,
		1, 1,-1,  1, 1,
		1,-1,-1,  0, 1,
		1, 1, 1,  1, 0,
		1,-1, 1,  0, 0,

		1, 1, 1,  1, 0,
		1, 1,-1,  1, 1,
		-1, 1,-1,  0, 1,
		1, 1, 1,  1, 0,
		-1, 1,-1, 0, 1,
		-1, 1, 1,  0, 0,
	};
	glGenVertexArrays(1, VertexArrayID);
	glBindVertexArray(VertexArrayID[0]);

	glGenBuffers(1, VBOs);

	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 38*5, vtxBufferData, GL_STATIC_DRAW);


	GLuint posID = glGetAttribLocation(programID, "inPos");
	glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, ((GLvoid*)(0)));
	glEnableVertexAttribArray(posID);

	GLuint texLID = glGetAttribLocation(programID, "texCoord");
	glVertexAttribPointer(texLID, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, ((GLvoid*)(sizeof(GLfloat) * 3)));
	glEnableVertexAttribArray(texLID);
 
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, texID);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.rows, image.cols, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    
    GLuint samplerID[1];
    glGenSamplers(1, samplerID);  
    glSamplerParameteri(samplerID[0], GL_TEXTURE_MAG_FILTER, GL_LINEAR);   
    glSamplerParameteri(samplerID[0], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindSampler(0, samplerID[0]);

	filterLoc = glGetUniformLocation(programID, "filterChoice");
	glUniform1i(filterLoc, filterChoice);

	//float viewPort[] = { 1.0f / g_windowWidth, 1.0f / g_windowHeight};
	GLuint viewPortLoc = glGetUniformLocation(programID, "u_inverseViewportDimensions");
	glUniform2f(viewPortLoc, 1.0f / g_windowWidth, 1.0f / g_windowHeight);

	createMenu();
	glutMouseFunc(mousePoint);
	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(2, VertexArrayID);

	return 1;
}

