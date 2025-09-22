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

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(100, 100); // �������� ��ġ ����
	glutInitWindowSize(800, 800); // �������� ũ�� ����
	glutCreateWindow("ComputerGraphics"); // ������ ���� (������ �̸�) 
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
		glutMotionFunc(Motion);
		glutMainLoop(); // �̺�Ʈ ó�� ����
	}
}

GLvoid drawScene() //--- �ݹ� �Լ�: ��� �ݹ� �Լ� 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ������
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
void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		for (int i = 0; i < rects.size(); i++) {
			GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
			GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
			if (rects[i].is_mouse_inside(ogl_x, ogl_y)) {
				std::cout << i << "��° rect Ŭ����\n";
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
				std::cout << left_clicked << "��° rect�� " << i << "��° rect �ȿ� ��\n";
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
		std::cout << "rect ����\n";
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.1f));
		break;
	}
	}
	glutPostRedisplay();
}