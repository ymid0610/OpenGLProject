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

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	for (int i = 0; i < MAX_RECT_COUNT; i++) {
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.05f));
	}
	eraser.size = 0.1f;
	eraser.red = 0.0f; eraser.green = 0.0f; eraser.blue = 0.0f;
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
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // ������
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	for (int i = 0; i < rects.size(); i++) {
		glColor3f(rects[i].red, rects[i].green, rects[i].blue);
		glRectf(rects[i].x1, rects[i].y1, rects[i].x2, rects[i].y2);
	}
	if (eraser.eraser) {
		glColor3f(eraser.red, eraser.green, eraser.blue);
		glRectf(eraser.x1, eraser.y1, eraser.x2, eraser.y2);
	}
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
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