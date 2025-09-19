#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//void Mouse(int button, int state, int x, int y);
//void Motion(int x, int y);

static bool left_button;

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(100, 100); // 윈도우의 위치 지정
	glutInitWindowSize(800, 800); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성 (윈도우 이름) 
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
		//glutMouseFunc(Mouse);
		//glutMotionFunc(Motion);
		glutMainLoop(); // 이벤트 처리 시작
	}
}

class CLASSRECT {
	public:
	GLfloat x, y, r;
	GLfloat red, green, blue;
	GLfloat x1, y1, x2, y2;

	CLASSRECT(float ix, float iy, float ir) {
		x = ix, y = iy; r = ir;
		x1 = x-r; y1 = y-r; x2 = x + r; y2 = y + r;
		random_color();
	}
	void random_color() {
		red = 0.0f + ((rand() % 100) * 0.01f);
		green = 0.0f + ((rand() % 100) * 0.01f);
		blue = 0.0f + ((rand() % 100) * 0.01f);
	}
};

static CLASSRECT rect1(0.5f, 0.5f, 0.5f);
static CLASSRECT rect2(-0.5f, 0.5f, 0.5f);
static CLASSRECT rect3(-0.5f, -0.5f, 0.5f);
static CLASSRECT rect4(0.5f, -0.5f, 0.5f);

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f); // 바탕색을 ‘blue’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.

	glColor3f(rect1.red, rect1.green, rect1.blue);
	glRectf(rect1.x1, rect1.y1, rect1.x2, rect1.y2);
	glColor3f(rect2.red, rect2.green, rect2.blue);
	glRectf(rect2.x1, rect2.y1, rect2.x2, rect2.y2);
	glColor3f(rect3.red, rect3.green, rect3.blue);
	glRectf(rect3.x1, rect3.y1, rect3.x2, rect3.y2);
	glColor3f(rect4.red, rect4.green, rect4.blue);
	glRectf(rect4.x1, rect4.y1, rect4.x2, rect4.y2);

	glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {

	}
}


