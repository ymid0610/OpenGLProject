#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define MAX_RECT_COUNT 30

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static bool left_clicked;
static bool right_clicked;

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

GLfloat ConvertScreenToOpenGLX(int x, int windowWidth);
GLfloat ConvertScreenToOpenGLY(int y, int windowHeight);

std::vector<YMRECT> rects;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(800, 800); // 윈도우의 크기 지정
	glutCreateWindow("ComputerGraphics"); // 윈도우 생성 (윈도우 이름) 
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW Initialized\n";
		glutDisplayFunc(drawScene); // 출력 함수의 지정
		glutReshapeFunc(Reshape); // 다시 그리기 함수 지정
		glutKeyboardFunc(Keyboard);
		glutMouseFunc(Mouse);
		glutMotionFunc(Motion);
		glutMainLoop(); // 이벤트 처리 시작
	}
}

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.


	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
	glutPostRedisplay();
}
void Motion(int x, int y) {
	if (left_clicked == true) {

	}
	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': {
		if (rects.size() >= MAX_RECT_COUNT) return;
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.2f));
		break;
	}
	}
	glutPostRedisplay();
}