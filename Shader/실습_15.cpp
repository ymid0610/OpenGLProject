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

//-------------------- 함수 선언부 --------------------
void make_vertexShaders();
void make_fragmentShaders();
void InitAxisBuffer();
void InitCubeFaceArrays();
void InitCubeBuffer();
char* filetobuf(const char* file);

GLuint make_shaderProgram();
GLvoid DrawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLvoid Update(float deltaTime);
GLvoid Timer(int value);

//-------------------- 전역 변수 --------------------
GLint width, height;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;
GLuint axisVao = 0, axisVbo[2];
GLuint cubeVao = 0, cubeVbo[2];

std::random_device rd;
std::mt19937 gen(rd());
float gl_x, gl_y;
static int drawFace = 0; // 0: 전체, 1~6: 해당 면만

//-------------------- 좌표축 데이터 --------------------
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

//-------------------- 큐브 데이터 (꼭짓점 좌표, 면 색상) --------------------
GLfloat cubeVertices[8][3] = {
    {-0.5f, -0.5f, -0.5f}, // 0
    { 0.5f, -0.5f, -0.5f}, // 1
    { 0.5f,  0.5f, -0.5f}, // 2
    {-0.5f,  0.5f, -0.5f}, // 3
    {-0.5f, -0.5f,  0.5f}, // 4
    { 0.5f, -0.5f,  0.5f}, // 5
    { 0.5f,  0.5f,  0.5f}, // 6
    {-0.5f,  0.5f,  0.5f}  // 7
};
// 각 면의 색상 (R,G,B)
GLfloat cubeColors[6][3] = {
    {1,0,0}, // 빨강
    {0,1,0}, // 초록
    {0,0,1}, // 파랑
    {1,1,0}, // 노랑
    {1,0,1}, // 마젠타
    {0,1,1}  // 시안
};

//-------------------- 큐브 면별 정점/색상 배열 (36개 정점) --------------------
// 각 면을 삼각형 2개(정점 6개)로 분할해서 면별 단색으로 그리기
GLfloat cubeFaceVertices[36][3];
GLfloat cubeFaceColors[36][3];

// 각 면의 삼각형 인덱스 (정점 인덱스, 반드시 반시계 방향으로!)
int faceTriIdx[6][6] = {
    // 뒤(0,1,2,3): z = -0.5, 앞에서 볼 때 반시계
    {0,1,2, 0,2,3},
    // 앞(4,5,6,7): z = +0.5, 앞에서 볼 때 반시계
    {4,5,6, 4,6,7},
    // 왼(0,4,7,3): x = -0.5, 왼쪽에서 볼 때 반시계
    {0,4,7, 0,7,3},
    // 오(1,5,6,2): x = +0.5, 오른쪽에서 볼 때 반시계
    {1,5,6, 1,6,2},
    // 위(3,2,6,7): y = +0.5, 위에서 볼 때 반시계
    {3,2,6, 3,6,7},
    // 아래(0,1,5,4): y = -0.5, 아래에서 볼 때 반시계
    {0,1,5, 0,5,4}
};

//-------------------- 큐브 면별 정점/색상 배열 생성 함수 --------------------
void InitCubeFaceArrays() {
    int idx = 0;
    for (int f = 0; f < 6; ++f) {
        for (int t = 0; t < 6; ++t) {
            int vi = faceTriIdx[f][t];
            for (int j = 0; j < 3; ++j)
                cubeFaceVertices[idx][j] = cubeVertices[vi][j];
            for (int j = 0; j < 3; ++j)
                cubeFaceColors[idx][j] = cubeColors[f][j];
            idx++;
        }
    }
}

//-------------------- 큐브 VAO/VBO 초기화 함수 --------------------
void InitCubeBuffer() {
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);

    glGenBuffers(2, cubeVbo);

    // 정점 버퍼
    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeFaceVertices), cubeFaceVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // 색상 버퍼
    glBindBuffer(GL_ARRAY_BUFFER, cubeVbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeFaceColors), cubeFaceColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

//-------------------- 축 VAO/VBO 초기화 함수 --------------------
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

//-------------------- 메인 함수 --------------------
int main(int argc, char** argv)
{
    width = 800;
    height = 800;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("실습 16");
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST); // 깊이 버퍼 사용
    glEnable(GL_CULL_FACE);  // 은면제거 활성화
    glCullFace(GL_BACK);     // 뒷면 제거
    glFrontFace(GL_CCW);     // 반시계 방향이 앞면

    make_vertexShaders();
    make_fragmentShaders();
    shaderProgramID = make_shaderProgram();

    InitAxisBuffer();
    InitCubeFaceArrays(); // 면별 정점/색상 배열 생성
    InitCubeBuffer();

    glutDisplayFunc(DrawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutTimerFunc(16, Timer, 0);
    glutMainLoop();
    return 0;
}

//-------------------- 화면 그리기 함수 --------------------
GLvoid DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 1. 변환 행렬 생성 (x축 30도, y축 -30도 회전)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0, 1, 0));
    GLuint loc = glGetUniformLocation(shaderProgramID, "modelTransform");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(model));

    // 2. 축 그리기
    glBindVertexArray(axisVao);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);

    // 3. 큐브 그리기 (면별 단색, 은면제거 적용)
    glBindVertexArray(cubeVao);
    if (drawFace) {
        for (int i = 0; i < 6; ++i) {
            if (drawFace & (1 << i)) {
                glDrawArrays(GL_TRIANGLES, i * 6, 6);
            }
        }
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

	glutSwapBuffers();
}

//-------------------- 기타 콜백 함수 --------------------
GLvoid Keyboard(unsigned char key, int x, int y) {
    if (key >= '1' && key <= '6') {
        drawFace ^= (1 << (key - '1')); // 토글: 누르면 켜고, 또 누르면 끔
    }
    else if (key == '0') {
        drawFace = 0; // 전체 면
    }
    glutPostRedisplay();
}
GLvoid Mouse(int button, int state, int x, int y) { glutPostRedisplay(); }
GLvoid Motion(int x, int y) { glutPostRedisplay(); }
GLvoid Timer(int value) { glutPostRedisplay(); }
GLvoid Update(float deltaTime) { glutPostRedisplay(); }
GLvoid Reshape(int w, int h) { glViewport(0, 0, w, h); }

//-------------------- 세이더 관련 함수 --------------------
void make_vertexShaders()
{
    GLchar* vertexSource;
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
    fragmentSource = filetobuf("fragment.glsl");
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
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
        return 0;
    }
    glUseProgram(shaderID);
    return shaderID;
}

//-------------------- 파일을 문자열로 읽어오는 함수 (세이더 소스 읽기용) --------------------
char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, 1, length, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}
