#include <iostream>
#include <gl/glew.h> //--- �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ���� { //--- ������ �����ϱ�
{
	glutInit(&argc, argv); // glut �ʱ�ȭ
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ���÷��� ��� ����
	glutInitWindowPosition(0, 0); // �������� ��ġ ����
	glutInitWindowSize(800, 600); // �������� ũ�� ����
	glutCreateWindow("Example1"); // ������ ���� (������ �̸�) 
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew �ʱ�ȭ
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); //--- ��� �Լ��� ����
	glutReshapeFunc(Reshape); //--- �ٽ� �׸��� �Լ� ����
	glutKeyboardFunc(Keyboard); //--- Ű���� �Է� �ݹ��Լ� ����
	glutMainLoop(); // �̺�Ʈ ó�� ����
}

static float r = 1.0f, g = 1.0f, b = 1.0f;

GLvoid drawScene() //--- �ݹ� �Լ�: ��� �ݹ� �Լ� 
{
	glClearColor(r ,g ,b ,1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // ������ ������ ��ü�� ĥ�ϱ�
	// �׸��� �κ� ����: �׸��� ���� �κ��� ���⿡ ���Եȴ�.
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ� 
{
	glViewport(0, 0, w, h);
}

static bool timer = false;

void TimerFunction(int value)
{
	if (timer) {
		r = 0.0f + ((rand() % 100) * 0.01f);
		g = 0.0f + ((rand() % 100) * 0.01f);
		b = 0.0f + ((rand() % 100) * 0.01f);

		glutPostRedisplay(); // ȭ�� �� ���
		glutTimerFunc(100, TimerFunction, 1); // Ÿ�̸��Լ� �� ����
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c': {
		r = 0.0f; g = 1.0f; b = 1.0f;
		break;
	}
	case 'm': {
		r = 1.0f; g = 0.0f; b = 1.0f;
		break;
	}
	case 'y': {
		r = 1.0f; g = 1.0f; b = 0.0f;
		break;
	}
	case 'a': {
		r = 0.0f + ((rand() % 100) * 0.01f);
		g = 0.0f + ((rand() % 100) * 0.01f);
		b = 0.0f + ((rand() % 100) * 0.01f);
		break;
	}
	case 'w': {
		r = 1.0f; g = 1.0f; b = 1.0f;
		break;
	}
	case 'k': {
		r = 0.0f; g = 0.0f; b = 0.0f;
		break;
	}
	case 't': {
		timer = TRUE;
		glutTimerFunc(100, TimerFunction, 1);
		break;
	}
	case 's': {
		timer = FALSE;
		break;
	}
	case 'q': {
		glutLeaveMainLoop();
		break;
	}
	}
	glutPostRedisplay();
}