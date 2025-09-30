//--- 필요한 헤더파일 선언
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include "ymshader.h"

#define MAX_OBJECT 10

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
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램

std::vector<VERTEX> Vertexs;
std::vector<unsigned int> List;
std::vector<OBJECT> Objects;
int v_count = 0; // 정점 개수

static int shape = ZUM;
int selected = -1; // 선택된 도형 인덱스

void main(int argc, char** argv)
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
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

	glUseProgram(shaderProgramID);
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, List.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		float cx = ConvertMouseToOGL_x(x, glutGet(GLUT_WINDOW_WIDTH));
		float cy = ConvertMouseToOGL_y(y, glutGet(GLUT_WINDOW_HEIGHT));
		if (Objects.size() >= MAX_OBJECT) return;
		AddShape(cx, cy, (SHAPE)shape);
		InitBuffer();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		float cx = ConvertMouseToOGL_x(x, glutGet(GLUT_WINDOW_WIDTH));
		float cy = ConvertMouseToOGL_y(y, glutGet(GLUT_WINDOW_HEIGHT));
		float minDist = 100.0f;
		selected = -1;
		for (int i = 0; i < Objects.size(); ++i) {
			int vi = List[Objects[i].start_idx];
			float dx = Vertexs[vi].x - cx;
			float dy = Vertexs[vi].y - cy;
			float dist = dx * dx + dy * dy;
			if (dist < minDist) { minDist = dist; selected = i; }
		}
	}
	glutPostRedisplay();
}
void Motion(int x, int y) {

	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p': shape = ZUM; break;
	case 'l': shape = LINE; break;
	case 't': shape = TRIANGLE; break;
	case 'r': shape = RECTANGLE; break;
	case 'c': Vertexs.clear(); List.clear(); Objects.clear(); v_count = 0; selected = -1; InitBuffer(); break;
	case 'w': // 위
		if (selected != -1) MoveObject(selected, 0.0f, 0.05f);
		break;
	case 'a': // 좌
		if (selected != -1) MoveObject(selected, -0.05f, 0.0f);
		break;
	case 's': // 아래
		if (selected != -1) MoveObject(selected, 0.0f, -0.05f);
		break;
	case 'd': // 우
		if (selected != -1) MoveObject(selected, 0.05f, 0.0f);
		break;
	case 'j': // 좌상
		if (selected != -1) MoveObject(selected, -0.05f, 0.05f);
		break;
	case 'k': // 우상
		if (selected != -1) MoveObject(selected, 0.05f, 0.05f);
		break;
	case 'n': // 좌하
		if (selected != -1) MoveObject(selected, -0.05f, -0.05f);
		break;
	case 'm': // 우하
		if (selected != -1) MoveObject(selected, 0.05f, -0.05f);
		break;
	case 'D': 
		PrintObjectInfo(selected);
		break;
	}
	InitBuffer();
	glutPostRedisplay();
}
void TimerFunction(int value) {

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * Vertexs.size(), Vertexs.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * List.size(), List.data(), GL_STATIC_DRAW);

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
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}
void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
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