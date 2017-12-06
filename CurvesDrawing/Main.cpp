
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define BEZIER 1
#define SPLINE 2
using namespace std;


int window_size = 480;
int curveChoice;

GLuint VertexArrayID;
GLuint myProgram;
GLuint aVertexLoc_bezier = -1;
GLuint aVertexLoc_spline = -1;
GLuint vertexBuffer[2];
GLfloat points[4][3] = {
	{ 0.f, 0.f, 0.f },
	{ 0.2f, 0.3f, 0.f },
	{ 0.3f, 0.2f, 0.f },
	{ 0.4f,1.f, 0.f }
};

GLfloat spline_points[2][3] = {
	{0.1f, 0.1f, 0.f},
	{0.4f, 0.3f, 0.f}
};

int curveTypeLoc = 1;
glm::mat4 uTrans;



GLuint LoadShaders(const char* vertex_file_path, const char* tess_control_file_path, const char* tess_eval_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint TessConShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	GLuint TessEvalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);

	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line)) {
			VertexShaderCode += "\n" + Line;
			//printf("%s\n", "reading vertex shader stream");
		}
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

	//Read the tess con shader code from the file
	string TessConShaderCode;
	ifstream TessConShaderStream(tess_control_file_path, ios::in);

	if (TessConShaderStream.is_open())
	{
		string Line = "";
		while (getline(TessConShaderStream, Line)) {
			TessConShaderCode += "\n" + Line;
			//printf("%s\n", "reading tess control shader stream");
		}
		TessConShaderStream.close();
	}

	//Compile Tess Control Shader
	printf("Compiling shader : %s\n", tess_control_file_path);
	char const* TessConSourcePointer = TessConShaderCode.c_str();
	glShaderSource(TessConShaderID, 1, &TessConSourcePointer, NULL);
	glCompileShader(TessConShaderID);

	//Check tess con Shader
	glGetShaderiv(TessConShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(TessConShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> TessShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(TessConShaderID, InfoLogLength, NULL, &TessShaderErrorMessage[0]);
		//fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	}
	
	//Read the tess eval shader code from the file
	string TessEvalShaderCode;
	ifstream TessEvalShaderStream(tess_eval_file_path, ios::in);

	if (TessEvalShaderStream.is_open())
	{
		string Line = "";
		while (getline(TessEvalShaderStream, Line)) {
			TessEvalShaderCode += "\n" + Line;
			//printf("%s\n", "reading tess eval shader stream");
		}
		TessEvalShaderStream.close();
	}

	//Compile Tess Eval Shader
	printf("Compiling shader : %s\n", tess_eval_file_path);
	char const* TessEvalSourcePointer = TessEvalShaderCode.c_str();
	glShaderSource(TessEvalShaderID, 1, &TessEvalSourcePointer, NULL);
	glCompileShader(TessEvalShaderID);

	//Check tess eval Shader
	glGetShaderiv(TessEvalShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(TessEvalShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength != 0) {
		vector<char> TessShaderErrorMessage(InfoLogLength);
		glGetShaderInfoLog(TessEvalShaderID, InfoLogLength, NULL, &TessShaderErrorMessage[0]);
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
	glAttachShader(ProgramID, TessConShaderID);
	glAttachShader(ProgramID, TessEvalShaderID);
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
	glDeleteShader(TessConShaderID);
	glDeleteShader(TessEvalShaderID);

	return ProgramID;
}


void renderScene(void)
{
	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	

	glUseProgram(myProgram);
	glBindVertexArray(VertexArrayID);


	glDrawArrays(GL_PATCHES, 0, 4);
	
	//glutPostRedisplay();
	//Double buffer
	glutSwapBuffers();
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

}

float getDistance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(fabs(x1 - x2), 2) + pow(fabs(y1 - y2), 2));
}



// min - max normalization
float* getNormalizedXY(int x, int y) {
	float *points = (float *)calloc(2, sizeof(float));
	float tempX = float(x);
	float tempY = float(y);
	float newMin = -1.0;
	float newMax = 1.0;
	float oldMin = 0;
	float oldMax = float(window_size);

	*points = (newMax - newMin)*(tempX - oldMin) / (oldMax - oldMin) + newMin;
	*(points + 1) = ((newMax - newMin)*(tempY - oldMin) / (oldMax - oldMin) + newMin) * (-1);

	return points;
}


float sign(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}


void mouseClick(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

		float* newPoints = getNormalizedXY(x, y);
		
		if (curveChoice == BEZIER) {
			points[3][0] = newPoints[0];
			points[3][1] = newPoints[1];
			points[3][3] = 0;

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
			glVertexAttribPointer(aVertexLoc_bezier, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));
		}
		
		else if (curveChoice == SPLINE) {
			spline_points[1][0] = newPoints[0];
			spline_points[1][1] = newPoints[1];
			spline_points[1][3] = 0;

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(spline_points), spline_points, GL_STATIC_DRAW);
			glVertexAttribPointer(aVertexLoc_spline, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));

		}
		

		glutPostRedisplay();
	}
}


void processMenuEvents(int option) {
	switch (option) {
	case BEZIER:
		curveChoice = BEZIER;
		glUniform1f(curveTypeLoc, BEZIER);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glVertexAttribPointer(aVertexLoc_bezier, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));
		printf("\n Bezier "); break;
	case SPLINE:
		curveChoice = SPLINE;
		glUniform1f(curveTypeLoc, SPLINE);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(spline_points), spline_points, GL_STATIC_DRAW);
		glVertexAttribPointer(aVertexLoc_spline, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));
		printf("\n Spline"); break;
	}
	glutPostRedisplay();
}




void createMenu() {
	int menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Bezier", BEZIER);
	glutAddMenuEntry("Spline", SPLINE);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



int main(int argc, char **argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(window_size, window_size);
	//This is used to define the name of the window.
	glutCreateWindow("Curve Editor");
	
	//call initization function
	init();

	//1.
	//Generate VAO




	// add mouse event listener
	glutMouseFunc(mouseClick);


	createMenu();

	//3. 
	GLuint programID = LoadShaders("VertexShader.txt", "TessControlShader.txt", "TessEvalShader.txt", "FragmentShader.txt");
	myProgram = programID;

	glUseProgram(programID);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	

	glutDisplayFunc(renderScene);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glGenBuffers(2, vertexBuffer);

	// bezier points
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	aVertexLoc_bezier = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc_bezier);
	glVertexAttribPointer(aVertexLoc_bezier, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));

	// spline points
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(spline_points), spline_points, GL_STATIC_DRAW);

	aVertexLoc_spline = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc_spline);
	glVertexAttribPointer(aVertexLoc_spline, 3, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));

	curveTypeLoc = glGetUniformLocation(programID, "curveType");

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

