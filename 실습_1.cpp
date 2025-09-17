#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 { //--- 윈도우 생성하기
{
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성 (윈도우 이름) 
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	glutDisplayFunc(drawScene); //--- 출력 함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

static float r = 1.0f, g = 1.0f, b = 1.0f;

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	glClearColor(r ,g ,b ,1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
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

		glutPostRedisplay(); // 화면 재 출력
		glutTimerFunc(100, TimerFunction, 1); // 타이머함수 재 설정
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