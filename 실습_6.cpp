#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include "ymheader.h"
#include <vector>

#define MAX_RECT_COUNT 5

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

static int left_clicked = -1;

void Mouse(int button, int state, int x, int y);

std::vector<YMRECT> rects;

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	for (int i = 0; i < MAX_RECT_COUNT; i++) {
		rects.push_back(YMRECT(RandomValue(), RandomValue(), 0.1f));
	}
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
		glutMouseFunc(Mouse);
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

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLfloat ogl_x = ConvertMouseToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertMouseToOpenGLY(y, 800);
		for (int i = 0; i < rects.size(); i++) {
			if (rects[i].is_mouse_inside(ogl_x, ogl_y)) {
				std::cout << i << "��° rect Ŭ����\n";
				left_clicked = i;
				break;
			}
		}
	}
	glutPostRedisplay();
}