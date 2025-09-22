#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define MAX_RECT_COUNT 30

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static int left_clicked = -1;
static bool right_clicked;

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);

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

	for (int i = 0; i < rects.size(); i++) {
		glColor3f(rects[i].red, rects[i].green, rects[i].blue);
		glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
	}

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < rects.size(); i++) {
			GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
			GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
			if (rects[i].is_mouse_inside(ogl_x, ogl_y)) {
				std::cout << i << "번째 rect 클릭됨\n";
				left_clicked = i;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (left_clicked < 0) {
			return;
		}
		for (int i = 0; i < rects.size(); i++) {
			if (rects[i].is_rect_inside(rects[left_clicked]) && (left_clicked >= 0 && left_clicked != i)) {
				std::cout << left_clicked << "번째 rect가 " << i << "번째 rect 안에 들어감\n";
				rects[i].union_rect(rects[left_clicked]);
				rects.erase(rects.begin() + left_clicked);
				left_clicked = -1;
				break;
			}
		}
		left_clicked = -1;
	}
	glutPostRedisplay();
}
void Motion(int x, int y) {
	if (left_clicked >= 0) {
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		rects[left_clicked].move_by_mouse(ogl_x,ogl_y);
	}
	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': {
		std::cout << "a key pressed\n";
		if (rects.size() >= 10) return;
		std::cout << "rect 생성\n";
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.1f));
		break;
	}
	}
	glutPostRedisplay();
}