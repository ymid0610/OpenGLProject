//--- �ʿ��� ������� ����
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include "ymshader.h"

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
void InitBuffer();
char* filetobuf(const char* file);
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint VAO, VBO_pos, EBO;
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; //--- ���̴� ���α׷�

std::vector<VERTEX> LineVertexs;
std::vector<VERTEX> TriVertexs;
std::vector<VERTEX> RectVertexs;
std::vector<VERTEX> PentaVertexs;
std::vector<VERTEX> MainVertexs;

std::vector<unsigned int> Lists;

int v_count = 0; // ���� ����

bool flag_line = false;
bool flag_rect = false;
bool flag_tri = false;
bool flag_penta = false;

void main(int argc, char** argv)
{
	std::vector<VERTEX> temp;
	temp = CreateTriangle(0.5f, 0.5f, 0.2f);
	for (int i = 0; i < temp.size(); i++) {
		MainVertexs.push_back(temp[i]);
	}
	for (int i = 1; i <= 5; ++i) {
		Lists.push_back(temp[0].id);           // 0
		Lists.push_back(temp[i].id);           // 1,2,3,4,5
		Lists.push_back(temp[(i % 5) + 1].id); // 2,3,4,5,1
	}
	for (int i = 0; i < temp.size(); i++) {
		TriVertexs.push_back(temp[i]);
	}

	temp = CreateTriangle(0.5f, 0.5f, 0.4f);
	for (int i = 0; i < temp.size(); i++) {
		TriVertexs.push_back(temp[i]);
	}
	temp = CreateRectangle(0.5f, 0.5f, 0.4f);
	for (int i = 0; i < temp.size(); i++) {
		RectVertexs.push_back(temp[i]);
	}
	temp = CreatePentagon(0.5f, 0.5f, 0.2f);
	for (int i = 0; i < temp.size(); i++) {
		PentaVertexs.push_back(temp[i]);
	}

	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutTimerFunc(16, TimerFunction, 1);
	glutMainLoop();
}
GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	InitBuffer();

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);


	glBegin(GL_LINES);
	glVertex3f(0.25f, 0.25f,0.0f);    // ù ��° ��
	glVertex3f(0.75f, 0.75f, 0.0f);    // �� ��° ��

	glEnd();

	glDrawElements(GL_TRIANGLES, Lists.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
	glutPostRedisplay();
}
void Motion(int x, int y) {

	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': {
		flag_rect = true;
		break;
	}
	case 't': {
		flag_tri = true;
		break;
	}
	}
	glutPostRedisplay();
}

void TimerFunction(int value) {
	if (flag_rect) {
		if (isCorrectMoved(PentaVertexs, MainVertexs)) {
			flag_rect = false;
		}
		else {
			std::vector<VERTEX> temp;
			temp = Move_Vertex(PentaVertexs, MainVertexs);
			MainVertexs.clear();
			for (int i = 0; i < temp.size(); i++) {
				MainVertexs.push_back(temp[i]);
			}
		}
	}
	else if (flag_tri) {
		if (isCorrectMoved(RectVertexs, MainVertexs)) {
			flag_tri = false;
		}
		else {
			std::vector<VERTEX> temp;
			temp = Move_Vertex(RectVertexs, MainVertexs);
			MainVertexs.clear();
			for (int i = 0; i < temp.size(); i++) {
				MainVertexs.push_back(temp[i]);
			}
		}
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}
void InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_pos);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * MainVertexs.size(), MainVertexs.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * Lists.size(), Lists.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(VERTEX), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}