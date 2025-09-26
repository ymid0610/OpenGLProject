#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define MAX_RECT_COUNT 30

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static bool left_clicked = FALSE;

void Mouse(int button, int state, int x, int y);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

std::vector<YMRECT> rects;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // �������� ũ�� ����
	glutCreateWindow("ComputerGraphics"); // ��3���� ���� (������ �̸�) 
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW Initialized\n";
		glutDisplayFunc(drawScene); // ��� �Լ��� ����
		glutReshapeFunc(Reshape); // �ٽ� �׸��� �Լ� ����
		glutKeyboardFunc(Keyboard);
		glutMouseFunc(Mouse);
		glutTimerFunc(16, TimerFunction, 1);
		glutMainLoop(); // �̺�Ʈ ó�� ����
	}
}

GLvoid drawScene() //--- �ݹ� �Լ�: ��� �ݹ� �Լ� 
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // ������
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.

	for (int i = 0; i < rects.size(); i++) {
		glColor3f(rects[i].red, rects[i].green, rects[i].blue);
		glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
	}

	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}
void TimerFunction(int value) {
	for (int i = 0; i < rects.size(); i++) {
		if (rects[i].move_type == CROSS) rects[i].move_cross();
		else if (rects[i].move_type == ZIGZAG) rects[i].move_zigzag();
		else if (rects[i].move_type == FOLLOW) rects[i].move_follow();
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
		for (int i = 0; i < rects.size(); i++) {
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