#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);

//void Mouse(int button, int state, int x, int y);
//void Motion(int x, int y);

static bool left_button;

void Mouse(int button, int state, int x, int y);
GLfloat ConvertScreenToOpenGLX(int x, int windowWidth);
GLfloat ConvertScreenToOpenGLY(int y, int windowHeight);

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
		glutMouseFunc(Mouse);
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
	bool IsPointInside(GLfloat px, GLfloat py) {
		if (px >= x1 && px <= x2 && py >= y1 && py <= y2) return true;
		else return false;
	}
	void Resize() {
		x1 = x - r; y1 = y - r; x2 = x + r; y2 = y + r;
	}
	void size_up() {
		if (r >= 1.0f) return;
		r += 0.1f;
		Resize();
	}
	void size_down() {
		if (r <= 0.2f) return;
		r -= 0.1f;
		Resize();
	}
};

static CLASSRECT rect1(0.5f, 0.5f, 0.5f);
static CLASSRECT rect2(-0.5f, 0.5f, 0.5f);
static CLASSRECT rect3(-0.5f, -0.5f, 0.5f);
static CLASSRECT rect4(0.5f, -0.5f, 0.5f);

static CLASSRECT background(0.0f, 0.0f, 0.0f);

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	glClearColor(background.red, background.green, background.blue, 1.0f); // 바탕색을 ‘blue’로 지정
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		std::cout << "left " << "x = " << x << " y = " << y << std::endl;
		GLfloat ogl_x = ConvertScreenToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertScreenToOpenGLY(y, 800);
		std::cout << "left" << "ogl_x = " << ogl_x << " ogl_y = " << ogl_y << std::endl;
		if (rect1.IsPointInside(ogl_x, ogl_y)) {
			std::cout << "rect1 clicked\n";
			rect1.random_color();
		}
		else if (rect2.IsPointInside(ogl_x, ogl_y)) {
			std::cout << "rect2 clicked\n";
			rect2.random_color();
		}
		else if (rect3.IsPointInside(ogl_x, ogl_y)) {
			std::cout << "rect3 clicked\n";
			rect3.random_color();
		}
		else if (rect4.IsPointInside(ogl_x, ogl_y)) {
			std::cout << "rect4 clicked\n";
			rect4.random_color();
		}
		else {
			std::cout << "background clicked\n";
			background.random_color();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		std::cout << "right " << "x = " << x << " y = " << y << std::endl;
		GLfloat ogl_x = ConvertScreenToOpenGLX(x, 800);
		GLfloat ogl_y = ConvertScreenToOpenGLY(y, 800);
		std::cout << "right " << "ogl_x = " << ogl_x << " ogl_y = " << ogl_y << std::endl;
		if (rect1.IsPointInside(ogl_x, ogl_y)) {
			rect1.size_down();
		}
		else if (rect2.IsPointInside(ogl_x, ogl_y)) {
			rect2.size_down();
		}
		else if (rect3.IsPointInside(ogl_x, ogl_y)) {
			rect3.size_down();
		}
		else if (rect4.IsPointInside(ogl_x, ogl_y)) {
			rect4.size_down();
		}
		else if (!rect1.IsPointInside(ogl_x, ogl_y) && (ogl_x > 0.0f && ogl_y > 0.0f)) {
			rect1.size_up();
		}
		else if (!rect2.IsPointInside(ogl_x, ogl_y) && (ogl_x < 0.0f && ogl_y > 0.0f)) {
			rect2.size_up();
		}
		else if (!rect3.IsPointInside(ogl_x, ogl_y) && (ogl_x < 0.0f && ogl_y < 0.0f)) {
			rect3.size_up();
		}
		else if (!rect4.IsPointInside(ogl_x, ogl_y) && (ogl_x > 0.0f && ogl_y < 0.0f)) {
			rect4.size_up();
		}
	}
	glutPostRedisplay();
}

GLfloat ConvertScreenToOpenGLX(int x, int windowWidth)
{
	return (2.0f * x) / windowWidth - 1.0f;
}

GLfloat ConvertScreenToOpenGLY(int y, int windowHeight)
{
	return 1.0f - (2.0f * y) / windowHeight;
}