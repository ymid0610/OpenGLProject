#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define MAX_RECT_COUNT 10

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static int left_clicked = -1;

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

std::vector<YMRECT> rects;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	rects.clear();
	rects.push_back(YMRECT(-0.5f, 0.0f, 0.1f));
	rects.push_back(YMRECT(-0.7f, 0.0f, 0.1f));
	rects.push_back(YMRECT(-0.3f, 0.0f, 0.1f));
	rects.push_back(YMRECT(-0.8f, 0.3f, 0.2f));
	rects.push_back(YMRECT(-0.3f, 0.3f, 0.2f));
	rects.push_back(YMRECT(-0.8f, -0.3f, 0.2f));
	rects.push_back(YMRECT(-0.3f, -0.3f, 0.2f));
	rects.push_back(YMRECT(-0.15f, -0.05f, 0.05f));
	rects.push_back(YMRECT(-0.15f, 0.05f, 0.05f));
	rects.push_back(YMRECT(-0.85f, 0.0f, 0.05f));
	for (int i = 0; i < rects.size(); i++) {
		rects[i].move_by_mouse(RandomHalfValue(), RandomValue());
	}
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
		glutTimerFunc(16, TimerFunction, 1);
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
		GLfloat temp_x1 = rects[i].CreatedX - rects[i].size;
		GLfloat temp_y1 = rects[i].CreatedY - rects[i].size;
		GLfloat temp_x2 = rects[i].CreatedX + rects[i].size;
		GLfloat temp_y2 = rects[i].CreatedY + rects[i].size;
		glRectf(temp_x1, temp_y1, temp_x2, temp_y2);
		glColor3f(0.3f, 0.3f, 0.3f);
		temp_x1 = rects[i].CreatedX - (rects[i].size - 0.01f);
		temp_y1 = rects[i].CreatedY - (rects[i].size - 0.01f);
		temp_x2 = rects[i].CreatedX + (rects[i].size - 0.01f);
		temp_y2 = rects[i].CreatedY + (rects[i].size - 0.01f);
		glRectf(temp_x1, temp_y1, temp_x2, temp_y2);
	}
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
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		for (int i = 0; i < rects.size(); i++) {
			if (rects[i].is_mouse_inside(ogl_x, ogl_y)) {
				left_clicked = i;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		if (left_clicked < 0) {
			return;
		}
		else {
			YMRECT temp;
			for (int i = 0; i < rects.size(); i++) {
				temp.x = rects[i].CreatedX;
				temp.y = rects[i].CreatedY;
				temp.size = rects[i].size;
				temp.remake_x1y1x2y2();
				if (rects[i].move_type == STOP) {
					if (rects[i].is_rect_inside(temp)) {
						rects[i].move_type = RESET;
						left_clicked = -1;
						break;
					}
				}
			}
			left_clicked = -1;
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	if (left_clicked >= 0) {
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		rects[left_clicked].move_by_mouse(ogl_x, ogl_y);
	}
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': {
		rects.clear();
		rects.push_back(YMRECT(-0.5f, 0.0f, 0.1f));
		rects.push_back(YMRECT(-0.7f, 0.0f, 0.1f));
		rects.push_back(YMRECT(-0.3f, 0.0f, 0.1f));
		rects.push_back(YMRECT(-0.8f, 0.3f, 0.2f));
		rects.push_back(YMRECT(-0.3f, 0.3f, 0.2f));
		rects.push_back(YMRECT(-0.8f, -0.3f, 0.2f));
		rects.push_back(YMRECT(-0.3f, -0.3f, 0.2f));
		rects.push_back(YMRECT(-0.15f, -0.05f, 0.05f));
		rects.push_back(YMRECT(-0.15f, 0.05f, 0.05f));
		rects.push_back(YMRECT(-0.85f, 0.0f, 0.05f));
		for (int i = 0; i < rects.size(); i++) {
			rects[i].move_by_mouse(RandomHalfValue(), RandomValue());
		}
		break;
	}
	case 'q': {
		glutLeaveMainLoop();
		break;
	}
	case 'm': {
		for (int i = 0; i < rects.size(); i++) {
			if (rects[i].move_type == STOP) {
				rects[i].move_type = RESET;
				break;
			}
		}
		break;
	}
	}
	glutPostRedisplay();
}
void TimerFunction(int value) {
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].move_type == RESET) {
			rects[i].move_reset();
		}
	}
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].x == rects[i].CreatedX && rects[i].y == rects[i].CreatedY) {
			if (rects[i].move_type == RESET) {
			std::cout << i << "번째 도형 맞춰짐\n";
			rects[i].move_type = FOLLOW;
			}
		}
	}	
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1);
}