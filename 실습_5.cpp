#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define MAX_RECT_COUNT 20

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static bool left_clicked = FALSE;

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);

std::vector<YMRECT> rects;
YMRECT eraser;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	for (int i = 0; i < MAX_RECT_COUNT; i++) {
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.05f));
	}
	eraser.size = 0.1f;
	eraser.red = 0.0f; eraser.green = 0.0f; eraser.blue = 0.0f;
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
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // 바탕색
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	for (int i = 0; i < rects.size(); i++) {
		glColor3f(rects[i].red, rects[i].green, rects[i].blue);
		glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
	}
	if (eraser.eraser) {
		glColor3f(eraser.red, eraser.green, eraser.blue);
		glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
	}
	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		eraser.move_by_mouse(ogl_x, ogl_y);
		eraser.eraser = true;
		left_clicked = TRUE;
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		eraser.eraser = false;
		eraser.red = 0.0f; eraser.green = 0.0f; eraser.blue = 0.0f;
		left_clicked = FALSE;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		if (rects.size() >= MAX_RECT_COUNT) return;
		rects.push_back(YMRECT(ConvertMouseToOpenGLX(x, 800), ConvertMouseToOpenGLY(y, 800), 0.05f));
		if (eraser.size <= 0.1f) return;
		eraser.size -= 0.02f;
		eraser.remake_x1y1x2y2();
	}
	glutPostRedisplay();
}
void Motion(int x, int y) {
	if (left_clicked) {
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		eraser.move_by_mouse(ogl_x, ogl_y);
		for (int i = 0; i < rects.size(); i++) {
			if (eraser.is_rect_inside(rects[i])) {
				eraser.red = rects[i].red; eraser.green = rects[i].green; eraser.blue = rects[i].blue;
				rects.erase(rects.begin() + i);
				i--;
				eraser.size += 0.02f;
				eraser.remake_x1y1x2y2();
			}
		}
	}
	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': {
		rects.clear();
		for (int i = 0; i < MAX_RECT_COUNT; i++) {
			rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.05f));
		}
		break;
	}
	}
	glutPostRedisplay();
}