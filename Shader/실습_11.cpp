#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include "GoodFunction.h"

char* filetobuf(const char* file);
void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
GLuint make_shaderProgram();
GLvoid DrawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Update(float deltaTime);
GLvoid Animation();
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

// 도형 구조체
struct Triangle {
    float x, y;
    std::vector<float> vertices;
    std::vector<float> colors;
};

// 전역 변수
float gl_x, gl_y; // 마우스 좌표
float size = 0.1f; // 도형 크기
std::random_device rd;
std::mt19937 gen(rd());

static std::vector<Triangle> triangles;

GLuint vao; // --- 버텍스 배열 객체 이름
GLuint vbo[2];

void CreateTriangleShape(float x, float y);

class SPIROR {
public:
	float CenterX1, CenterY1; // 원 스파이럴 중심 좌표1
	float CenterX2, CenterY2; // 원 스파이럴 중심 좌표2
    float x, y;
	float angle = 0.0f; // 현재 각도
	float radius = 0.0f; // 현재 반지름
	float angle_step = 5.0f; // 각도 증가량 (도 단위) 720 / 5 = 144
	float radius_step = 0.001f; // 반지름 증가량
	int direction = 0; 
	bool spiral_done = false;

    std::vector<float> points; // 각 스파이럴의 점 배열

    void move_spiror() {

        if (direction == 0) {

            if (angle >= 720.0f) {
				direction = 1;
                return;
            }

            x = CenterX1 + radius * cos(angle * 3.14159 / 180.0f);
            y = CenterY1 + radius * sin(angle * 3.14159 / 180.0f);
            angle += angle_step;
            radius += radius_step;

        }
        else if (direction == 1) {

			if (angle <= 0.0f) {
				direction = 2;
                angle = 180.0f;
				spiral_done = true;
				return;
			}

			x = CenterX2 - radius * cos(angle * 3.14159 / 180.0f);
			y = CenterY2 - radius * sin(angle * 3.14159 / 180.0f);
			angle -= angle_step;
			radius -= radius_step;
        }

        points.push_back(x);
        points.push_back(y);
        points.push_back(0.0f);
    }

};

std::vector<SPIROR> spirors;

static bool flag = true;

std::uniform_real_distribution<float> cor(0.0f, 1.0f);
static float rc = cor(gen);
static float gc = cor(gen);
static float bc = cor(gen);

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    width = 800;
    height = 800;
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("실습 8");
    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    glewInit();
    //--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
    make_vertexShaders(); //--- 버텍스 세이더 만들기
    make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
    shaderProgramID = make_shaderProgram();
    InitBuffer();

    //--- 세이더 프로그램 만들기
    glutDisplayFunc(DrawScene); //--- 출력 콜백 함수
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutTimerFunc(16, Timer, 0); // 타이머 실행시키고 시작
    glutMainLoop();
}
GLvoid DrawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	glClearColor(rc, gc, bc, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    if (flag) {
        // 모든 스파이럴 궤적을 선으로 그림
        for (const auto& spiror : spirors) {
            if (spiror.points.size() < 6) continue; // 최소 2점 이상
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, spiror.points.size() * sizeof(float), spiror.points.data());

            // 색상: 파란색
            std::vector<float> line_colors(spiror.points.size(), 0.0f);
            for (size_t i = 0; i < spiror.points.size() / 3; ++i) {
                line_colors[i * 3 + 2] = 1.0f;
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, line_colors.size() * sizeof(float), line_colors.data());

            glDrawArrays(GL_LINE_STRIP, 0, spiror.points.size() / 3);
        }
    }
    else {
        for (const auto& triangle1 : triangles) {
            glBindVertexArray(vao);

            // 버텍스 데이터 업데이트
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.vertices.size() * sizeof(float), triangle1.vertices.data());

            // 색상 데이터 업데이트
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

            glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.colors.size() * sizeof(float), triangle1.colors.data());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glutSwapBuffers(); // 화면에 출력하기
}
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y) {

    std::uniform_real_distribution<float> loc(-1.0f, 1.0f);
    if (key == 'q' || key == 'Q') {
        std::cout << "종료" << std::endl;
        exit(0);
    }
    else if (key == 'c' || key == 'C') {
        triangles.clear();
		spirors.clear();
    }
    else if (key == '1') {
        float sp_x = loc(gen);
        float sp_y = loc(gen);
        spirors.push_back(SPIROR{ sp_x, sp_y, sp_x + 0.288f, sp_y });
    }
	else if (key == '2') {
		for (int i = 0; i < 2; i++) {
			float sp_x = loc(gen);
			float sp_y = loc(gen);
			spirors.push_back(SPIROR{ sp_x, sp_y, sp_x + 0.288f, sp_y });
		}
	}
	else if (key == '3') {
		for (int i = 0; i < 3; i++) {
			float sp_x = loc(gen);
			float sp_y = loc(gen);
			spirors.push_back(SPIROR{ sp_x, sp_y, sp_x + 0.288f, sp_y });
		}
	}
	else if (key == '4') {
		for (int i = 0; i < 4; i++) {
			float sp_x = loc(gen);
			float sp_y = loc(gen);
			spirors.push_back(SPIROR{ sp_x, sp_y, sp_x + 0.288f, sp_y });
		}
	}
	else if (key == '5') {
		for (int i = 0; i < 5; i++) {
			float sp_x = loc(gen);
			float sp_y = loc(gen);
			spirors.push_back(SPIROR{ sp_x, sp_y, sp_x + 0.288f, sp_y });
		}
	}
    else {
        if (key == 'p' || key == 'P') {
            flag = true;
        }
        else if (key == 'l' || key == 'L') {
            flag = false;
        }
        glutPostRedisplay();
        return;
    }
    rc = cor(gen);
    gc = cor(gen);
    bc = cor(gen);
    glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		if (spirors.size() < 1) {
            gl_x = convertX(x, width);
            gl_y = convertY(y, height);
            spirors.push_back(SPIROR{ gl_x, gl_y, gl_x + 0.288f, gl_y });
            rc = cor(gen);
            gc = cor(gen);
            bc = cor(gen);
		}
        else {
            std::uniform_real_distribution<float> loc(-1.0f, 1.0f);
			float sp_x = loc(gen);
			float sp_y = loc(gen);
			spirors.push_back(SPIROR{ sp_x, sp_y, sp_x+0.288f, sp_y });
            rc = cor(gen);
            gc = cor(gen);
            bc = cor(gen);
        }
    }
    glutPostRedisplay();
}
GLvoid Motion(int x, int y) {
    glutPostRedisplay();
}
GLvoid Timer(int value) {

	for (auto& spiror : spirors) {
		if (spiror.spiral_done) continue;
        spiror.move_spiror();
        CreateTriangleShape(spiror.x, spiror.y);
	}
    glutPostRedisplay();
	glutTimerFunc(16, Timer, 0);
}

void CreateTriangleShape(float x, float y) {
    Triangle triangle;
    triangle.x = x;
    triangle.y = y;

    size = 0.002f;

    triangle.vertices = {
        x - size, y - size / 2, 0.0f,  // 왼쪽 하단
        x + size, y - size / 2, 0.0f,  // 오른쪽 하단
        x, y + size * 2, 0.0f          // 상단
    };

    triangle.colors = {
        0.0f, 0.0f, 0.1f,
        0.0f, 0.0f, 0.1f,
        0.0f, 0.0f, 0.1f
    };

    triangles.push_back(triangle);
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb"); // Open file for reading
    if (!fptr) // Return NULL on failure
        return NULL;
    fseek(fptr, 0, SEEK_END); // Seek to the end of the file
    length = ftell(fptr); // Find out how many bytes into the file we are
    buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
    fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
    fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
    fclose(fptr); // Close the file
    buf[length] = 0; // Null terminator
    return buf; // Return the buffer
}
void InitBuffer() {
    if (flag) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(2, vbo);

        // 버텍스 버퍼 (스파이럴용: 충분히 크게)
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 3000 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // 색상 버퍼
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, 3000 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }
    else {
        glGenVertexArrays(1, &vao); //--- vao 지정 및 할당
        glBindVertexArray(vao); //--- vao 바인딩
        glGenBuffers(2, vbo); //--- vbo 지정 및 할당

        // 버텍스 버퍼 (최대 6개 정점 - 사각형용)
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- attribute 0: 버텍스 위치
        glEnableVertexAttribArray(0); //--- attribute 0 사용 설정

        // 색상 버퍼
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- attribute 1: 버텍스 색상
        glEnableVertexAttribArray(1); //--- attribute 1 사용 설정
    }
}
void make_vertexShaders()
{
    GLchar* vertexSource;
    //--- 버텍스 세이더 읽어 저장하고 컴파일 하기
    //--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    GLchar* fragmentSource;
    //--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
    fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
GLuint make_shaderProgram()
{
    GLuint shaderID;
    shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
    glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
    glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
    glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
    glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
    glDeleteShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return 0; // false 대신 0 반환
    }
    glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기

    return shaderID;
}