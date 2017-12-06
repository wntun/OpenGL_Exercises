
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glut.h>


using namespace std;


int window_size = 480;


GLuint aVertexLoc = -1;
GLint colorLoc = -1;

GLuint vertexBuffer;
GLfloat vertices[4] = {
	0,0,
	0.5,0.5
};
bool vertexTurn = 0;
int colorTurn = 0; // r->g->b


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


	//define the size of point and draw a point.
	glDrawArrays(GL_LINES, 0, 2);

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

void mouseClick(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

		// min-max normalization
		float tempX = float(x);
		float tempY = float(y);
		float newMin = -1.0;
		float newMax = 1.0;
		float oldMin = 0;
		float oldMax = float(window_size);

		if (vertexTurn == 1) {
			vertices[0] = (newMax - newMin)*(tempX - oldMin) / (oldMax - oldMin) + newMin;
			vertices[1] = ((newMax - newMin)*(tempY - oldMin) / (oldMax - oldMin) + newMin) * (-1);
		}
		else {
			vertices[2] = (newMax - newMin)*(tempX - oldMin) / (oldMax - oldMin) + newMin;
			vertices[3] = ((newMax - newMin)*(tempY - oldMin) / (oldMax - oldMin) + newMin) * (-1);
		}
		vertexTurn = !vertexTurn;	
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(aVertexLoc, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		int r, g, b;
		switch (colorTurn) {
		case 0: // Red to Green
			r = 0;
			g = 1;
			b = 0;
			colorTurn = 1;
			break;
		case 1: // Green to blue
			r = 0;
			g = 0;
			b = 1;
			colorTurn = 2;
			break;
		case 2:
			r = 1;
			g = 0;
			b = 0;
			colorTurn = 0;
			break;
		default:
			printf("\n ColorTurn variable problem!\n");
			break;
		}

		

		glUniform4f(colorLoc, r, g, b, 0);
	} 
	
	
	

	
	
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
	glutCreateWindow("Simple OpenGL Window");

	//call initization function
	init();
	
	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	

	// add mouse event listener
	glutMouseFunc(mouseClick);



	//3. 
	GLuint programID = LoadShaders("VertexShader.txt", "FragmentShader.txt");



	

	//aPosLoc = glGetAttribLocation(programID, "a_Pos");

	
	

	glUseProgram(programID);
	glutDisplayFunc(renderScene);


	//buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4, vertices, GL_STATIC_DRAW);

	aVertexLoc = glGetAttribLocation(programID, "vertexPos");
	glEnableVertexAttribArray(aVertexLoc);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(aVertexLoc, 2, GL_FLOAT, GL_FALSE, 0, ((void*)(0)));

	colorLoc = glGetUniformLocation(programID, "color_uniform");
	glUniform4f(colorLoc, 1, 0, 0, 0);
	
	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

