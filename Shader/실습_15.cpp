#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include "GoodFunction.h"

void make_vertexShaders();
void make_fragmentShaders();
void InitBuffer();
void InitAxisBuffer();
char* filetobuf(const char* file);

GLuint make_shaderProgram();
GLvoid DrawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Update(float deltaTime);
GLvoid Timer(int value);

//--- 필요한 변수 선언
GLint width, height;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLuint vao; // --- 버텍스 배열 객체 이름
GLuint vbo[2];
GLuint axisVao = 0, axisVbo[2];

// 전역 변수
std::random_device rd;
std::mt19937 gen(rd());
float gl_x, gl_y; // 마우스 좌표

// 축 좌표와 색상
GLfloat axisVertices[] = {
    -1,0,0,  1,0,0,   // x축
     0,-1,0, 0,1,0,   // y축
     0,0,-1, 0,0,1    // z축
};
GLfloat axisColors[] = {
    1,0,0, 1,0,0,     // x축 빨강
    0,1,0, 0,1,0,     // y축 초록
    0,0,1, 0,0,1      // z축 파랑
};

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    width = 800;
    height = 800;

    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("실습 16");
    //--- GLEW 초기화하기
    glewExperimental = GL_TRUE;
    glewInit();
    //--- 세이더 읽어와서 세이더 프로그램 만들기: 사용자 정의함수 호출
    make_vertexShaders(); //--- 버텍스 세이더 만들기
    make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
    shaderProgramID = make_shaderProgram();
    InitBuffer();
    InitAxisBuffer();

    //--- 세이더 프로그램 만들기
    glutDisplayFunc(DrawScene); //--- 출력 콜백 함수
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutTimerFunc(16, Timer, 0); // 타이머 실행시키고 시작
    glutMainLoop();
}

GLvoid DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 1. 변환 행렬 생성
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0, 1, 0));

    // 2. uniform으로 전달
    GLuint loc = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

    // 3. 축 그리기
    glBindVertexArray(axisVao);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    glutSwapBuffers();
}

GLvoid Keyboard(unsigned char key, int x, int y) {

    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {

    glutPostRedisplay();
}

GLvoid Motion(int x, int y) {

    glutPostRedisplay();
}

GLvoid Timer(int value) {

    glutPostRedisplay();
}

GLvoid Update(float deltaTime) {

    glutPostRedisplay();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
}

void InitBuffer() {
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
void InitAxisBuffer() {
    glGenVertexArrays(1, &axisVao);
    glBindVertexArray(axisVao);

    glGenBuffers(2, axisVbo);

    glBindBuffer(GL_ARRAY_BUFFER, axisVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, axisVbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisColors), axisColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
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
