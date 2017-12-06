
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

#define SAVE 0
#define LOAD 1
#define SHAPE 2
#define COLOR 3
#define POINT 4
#define LINE 5
#define TRIANGLE 6
using namespace std;


int window_size = 480;
int shapeOption;

GLuint VertexArrayID;
GLuint myProgram;
GLuint aVertexLoc_triangle = -1;
GLuint aVertexLoc_line = -1;
GLuint aVertexLoc_point = -1;
GLint colorLoc = -1;
GLint transLoc = -1;

GLuint vertexBuffer[3];
GLfloat lines[4] = {
	0,0,
	0.5,0.5
};
GLfloat points[2] = { 0,0 };
GLfloat triangles[6] = {-0.5, -0.5, -0.5, 0.5, 0.5, 0};
bool lineTurn = 0;
int triangleTurn = 1;
float r, g, b;
int objectSelected = -1;
float tx, ty;
glm::mat4 uTrans;



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
	glClear(GL_COLOR_BUFFER_BIT);
	//Let's draw something here


	/*glBindVertexArray(VertexArrayID[0]);
	glDrawArrays(GL_LINES, 0, 2);

	glBindVertexArray(VertexArrayID[1]);
	glDrawArrays(GL_POINTS, 0, 1);

	glBindVertexArray(VertexArrayID[2]);
	glDrawArrays(GL_TRIANGLES, 0, 3);*/

	glUseProgram(myProgram);
	glBindVertexArray(VertexArrayID);
	//glUniformMatrix4fv(transLoc, 1, GL_FALSE, &uTrans[0][0]);
	

	/*glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_POINTS, 0, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);*/
	switch (shapeOption) {
	case LINE:
		//glBindVertexArray(VertexArrayID[0]);	
		//printf("\nDrawing line");
		glDrawArrays(GL_LINES, 0, 2);
		break;
	case POINT:
		//glBindVertexArray(VertexArrayID[1]);
		//printf("\nDrawing point");
		glDrawArrays(GL_POINTS, 0, 1);
		break;
	case TRIANGLE:
		//glBindVertexArray(VertexArrayID[2]);
		//printf("\nDrawing triangle");
		glDrawArrays(GL_TRIANGLES, 0, 3);
		break;
	}
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
	r = 0;
	b = 0;
	g = 0;
	tx = 0;
	ty = 0;

}

float getDistance(float x1, float y1, float x2, float y2) {
	return sqrt(pow(fabs(x1 - x2),2) + pow(fabs(y1 - y2),2));
}

bool selectLine(float x, float y) {
	bool selected = false;
	float newLineDistance = getDistance(lines[0], lines[1], x, y) + getDistance(lines[2], lines[3], x, y);
	if (newLineDistance == getDistance(lines[0], lines[1], lines[2], lines[3])){
		selected = true;
		if (objectSelected != LINE) {
			printf("\nSelected line");
			glUniform4f(colorLoc, 0, 0, 1, 0);
			//selected = true;
			objectSelected = LINE;
		}
		else {
			printf("\nUnselected line");
			//selected = true;
			objectSelected = -1;
			glUniform4f(colorLoc, r, g, b, 0);
		}		
	}
	return selected;
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
	*(points+1) = ((newMax - newMin)*(tempY - oldMin) / (oldMax - oldMin) + newMin) * (-1);

	return points;
}


void drawLine(int x, int y) {
	float* newPoints = getNormalizedXY(x, y);
	float newPointX = *newPoints;
	float newPointY = *(newPoints + 1);

	float dx = (newPointX - lines[0]);
	float dy = (newPointY - lines[1]);

	bool selected = selectLine(newPointX, newPointY);
	if (!selected) { // just draw a line
		if (objectSelected == LINE) { // moving
			float dx = (newPointX - lines[0]);
			float dy = (newPointY - lines[1]);
			printf("\nMoving point using translate");
			uTrans = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0));
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, newPoints, GL_STATIC_DRAW);
			glVertexAttribPointer(aVertexLoc_line, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, &uTrans[0][0]);
			glutPostRedisplay();
			objectSelected = -1;
		}
		else {
			objectSelected = -1;
			if (lineTurn == 1) {
				selected = selectLine(newPointX, newPointY);
				lines[0] = newPointX;
				lines[1] = newPointY;
				//lineTurn = 2;
			}
			else {
				lines[2] = newPointX;
				lines[3] = newPointY;
				//lineTurn = 1;
			}

			lineTurn = !lineTurn;
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, lines, GL_STATIC_DRAW);
			glVertexAttribPointer(aVertexLoc_line, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
			glUniform4f(colorLoc, r, g, b, 0);
			glutPostRedisplay();
		}		
	}
	free(newPoints);
}

bool selectPoint(float x, float y) {
	bool selected = false;
	if (x == points[0] && y == points[1]) {
		if (objectSelected != POINT) {
			printf("\nSelected point");
			glUniform4f(colorLoc, 0, 0, 1, 0);
			selected = true;
			objectSelected = POINT;
		}
		else {
			printf("\nUnselected point");
			selected = false;
			objectSelected = -1;
			glUniform4f(colorLoc, r, g, b, 0);
		}		
	}

	return selected;
}

void drawPoint(int x, int y) {
	float* newPoints = getNormalizedXY(x, y);
	float newPointX = *newPoints;
	float newPointY = *(newPoints + 1);

	if (!selectPoint(newPointX, newPointY)) {
		if (objectSelected == POINT) {
			/*float dx = (newPointX - points[0]);
			float dy = (newPointY - points[1]);
			printf("\nMoving point using translate");
			uTrans = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0));
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, &uTrans[0][0]);
			glutPostRedisplay();*/
			printf("\nMoving point using translate");
			points[0] = newPointX;
			points[1] = newPointY;			
		}
		else {
			points[0] = newPointX;
			points[1] = newPointY;
			objectSelected = -1;
			glUniform4f(colorLoc, r, g, b, 0);
						
		}
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2, newPoints, GL_STATIC_DRAW);
		glVertexAttribPointer(aVertexLoc_point, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
		glutPostRedisplay();
		
	}	
	free(newPoints);
}

float sign(float x1, float y1, float x2, float y2, float x3, float y3) {
	return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
}

bool selectTriangle(float x, float y) {
	bool selected = false;

	bool b1, b2, b3;
	b1 = sign(x, y, triangles[0], triangles[1], triangles[2], triangles[3]) < 0.0f;
	b2 = sign(x, y, triangles[2], triangles[3], triangles[4], triangles[5]) < 0.0f;
	b3 = sign(x, y, triangles[4], triangles[5], triangles[0], triangles[1]) < 0.0f;

	if ((b1 == b2) && (b2 == b3)) {
		if (objectSelected != TRIANGLE) {
			printf("\nSelected triangle");
			glUniform4f(colorLoc, 0, 0, 1, 0);
			selected = true;
			objectSelected = TRIANGLE;
		}
		else {
			selected = true;
			objectSelected = -1;
			glUniform4f(colorLoc, r, g, b, 0);
		}
	}
	

	return selected;
}

void drawTriangle(int x, int y) {
	float* newPoints = getNormalizedXY(x, y);
	float newPointX = *newPoints;
	float newPointY = *(newPoints + 1);

	bool selected = selectTriangle(newPointX, newPointY);
	if (!selected) {
		if (objectSelected == TRIANGLE) {
			float dx = (newPointX - points[0]);
			float dy = (newPointY - points[1]);
			printf("\nMoving point using translate");
			uTrans = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0));
			glUniformMatrix4fv(transLoc, 1, GL_FALSE, &uTrans[0][0]);
			glutPostRedisplay();
		}
		else {
			objectSelected = -1;
			switch (triangleTurn) {
			case 1:
				triangles[0] = newPointX;
				triangles[1] = newPointY;
				triangleTurn = 2;
				break;
			case 2:
				triangles[2] = newPointX;
				triangles[3] = newPointY;
				triangleTurn = 3;
				break;
			case 3:
				triangles[4] = newPointX;
				triangles[5] = newPointY;
				triangleTurn = 1;
				break;
			}
			glUniform4f(colorLoc, r, g, b, 0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);
			glVertexAttribPointer(aVertexLoc_triangle, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
			glutPostRedisplay();
		}
		
	}
	free(newPoints);	
}


void mouseClick(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		switch (shapeOption) {
		case POINT:
			drawPoint(x, y);
			break;
		case LINE:
			drawLine(x, y);
			break;
		case TRIANGLE:
			drawTriangle(x, y);
			break;
		}
	}
}

void askRGBColor() {
	printf("\nRGB Color Input (0-255)");
	printf("\nRed value: ");
	scanf_s("%f", &r);
	printf("Green value: ");
	scanf_s("%f", &g);
	printf("Blue value: ");
	scanf_s("%f", &b);
	
	r = r / 255.0f;
	g = g / 255.0f;
	b = b / 255.0f;

	glUniform4f(colorLoc, r, g, b, 0);
	if (objectSelected != -1) {
		glutPostRedisplay();
	}	
}

//void saveFile() {
//	printf("\nEnter the new file name to save:");
//	char fileName[20];
//	scanf_s(fileName);
//
//	FILE *file = fopen(fileName, 'wb');
//
//	fclose(file);
//}

void loadFile() {
	
}

void processMenuEvents(int option) {
	switch (option) {
	case SHAPE:
		printf("\nShape"); break;
	case COLOR:
		askRGBColor();
		printf("\nColor"); break;
	case SAVE:
		printf("\nSave file"); break;
	case LOAD:
		printf("\nLoad file"); break;
	}
}

void processShapeMenuEvent(int option) {
	glUniform4f(colorLoc, r, g, b, 0);
	switch (option) {
	case POINT:
		printf("\npoint");
		shapeOption = POINT;
		break;
	case LINE:
		printf("\nline"); 
		shapeOption = LINE;
		break;
	case TRIANGLE:
		printf("\ntriangle"); 
		shapeOption = TRIANGLE;
		break;
	}
}



void createMenu() {
	int shapeMenu = glutCreateMenu(processShapeMenuEvent);
	glutAddMenuEntry("Point", POINT);
	glutAddMenuEntry("Line", LINE);
	glutAddMenuEntry("Triangle", TRIANGLE);

	int menu = glutCreateMenu(processMenuEvents);
	glutAddSubMenu("Shape", shapeMenu);
	glutAddMenuEntry("Color", COLOR);
	glutAddMenuEntry("Load file", LOAD);
	glutAddMenuEntry("Save file", SAVE);
	
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
	glutCreateWindow("Graphic Editor");

	//call initization function
	init();

	//1.
	//Generate VAO
	
	


	// add mouse event listener
	glutMouseFunc(mouseClick);

	
	createMenu();

	//3. 
	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");
	myProgram = programID;

	glUseProgram(programID);
	glutDisplayFunc(renderScene);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	//buffer for point
	glGenBuffers(1, &vertexBuffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, points, GL_STATIC_DRAW);

	aVertexLoc_point = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc_point);
	glVertexAttribPointer(aVertexLoc_point, 2, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));

	
	//buffer for line
	glGenBuffers(2, &vertexBuffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4, lines, GL_STATIC_DRAW);

	aVertexLoc_line = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc_line);
	glVertexAttribPointer(aVertexLoc_line, 2, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));
	
	//buffer for triangle
	glGenBuffers(3, &vertexBuffer[2]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 6, triangles, GL_STATIC_DRAW);

	aVertexLoc_triangle = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc_triangle);
	glVertexAttribPointer(aVertexLoc_triangle, 2, GL_FLOAT, GL_FALSE, 0, ((GLvoid*)(0)));

		
	colorLoc = glGetUniformLocation(programID, "color_uniform");
	//glUniform4f(colorLoc, r, g, b, 0);

	transLoc = glGetUniformLocation(programID, "uTransform");
	//glUniform4f(transLoc, tx, ty, 0, 0.0);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

