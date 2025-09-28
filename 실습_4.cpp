#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MAX_RECT_COUNT 5

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static bool left_clicked = FALSE;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

std::vector<YMRECT> rects;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("ComputerGraphics"); // 윈3도우 생성 (윈도우 이름) 
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
		glutTimerFunc(16, TimerFunction, 1);
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

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}
void TimerFunction(int value) {
	if (rects.size() == 0) {
		glutPostRedisplay();
		glutTimerFunc(16, TimerFunction, 1);
		return;
	}
	FLOATXY temp;
	if (rects[0].dix >= 1.0f && rects[0].diy >= 1.0f) {
		temp = { rects[0].x - rects[0].size, rects[0].y - rects[0].size };
	}
	else if (rects[0].dix <= -1.0f && rects[0].diy >= 1.0f) {
		temp = { rects[0].x + rects[0].size, rects[0].y - rects[0].size };
	}
	else if (rects[0].dix >= 1.0f && rects[0].diy <= -1.0f) {
		temp = { rects[0].x - rects[0].size, rects[0].y + rects[0].size };
	}
	else if (rects[0].dix <= -1.0f && rects[0].diy <= -1.0f) {
		temp = { rects[0].x + rects[0].size, rects[0].y + rects[0].size };
	}
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].move_type == CROSS) rects[i].move_cross();
		else if (rects[i].move_type == ZIGZAG) rects[i].move_zigzag();
		else if (rects[i].move_type == FOLLOW) {
			if (i == 0) continue;
			temp = rects[i].move_follow(temp.x, temp.y, rects[i-1].dix, rects[i-1].diy);
			rects[i].move_type = rects[0].move_type;
			rects[i].dix = rects[0].dix;
			rects[i].diy = rects[0].diy;
		}
		else if (rects[i].move_type == RESET) rects[i].move_reset();
		else {

		}
		if (rects[i].change_type == SIZECHANGE) rects[i].random_size();
		else if (rects[i].change_type == COLORCHANGE) rects[i].random_color();
	}
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (rects.size() >= MAX_RECT_COUNT) return;
		rects.push_back(YMRECT(ConvertMouseToOpenGLX(x, WINDOW_WIDTH), ConvertMouseToOpenGLY(y, WINDOW_HEIGHT), 0.1f));
	}
	glutPostRedisplay();
}
void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].move_type = CROSS;
		}
		break;
	}
	case '2': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].move_type = ZIGZAG;
		}
		break;
	}
	case '3': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].change_type = SIZECHANGE;
		}
		break;
	}
	case '4': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].change_type = COLORCHANGE;
		}
		break;
	}
	case '5': {
		for (int i = 1; i < rects.size(); i++) {
			rects[i].move_type = FOLLOW;
		}
		break;
	}
	case 's': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].move_type = STOP;
			rects[i].change_type = NOTHING;
		}
		break;
	}
	case 'm': {
		for (int i = 0; i < rects.size(); i++) {
			rects[i].move_type = RESET;
		}
		break;
	}
	case 'r': {
		rects.clear();
		break;
	}
	case 'q': {
		glutLeaveMainLoop();
		break;
	}
	}
	glutPostRedisplay();
}