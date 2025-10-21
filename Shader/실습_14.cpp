// unfirom 변수는 만들었으면 사용해야함
// 버퍼 위치가 바뀌면 계속 보내줘야함

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

// 도형 클래스
class TRIANGLE {
public:
    float x, y;
    float vx = 0.0f, vy = 0.0f;
    float size = 0.2;

    std::vector<float> vertices;
    std::vector<float> colors;
};

float deltaTime = 0.0f;

// bool
bool animation = false; // 애니메이션 상태 변수
bool TriangleShape = false;
bool wireframeMode = false; // 와이어프레임 모드 여부
bool TurnRight = false;
bool TurnLeft = false;

// 전역 변수
float gl_x, gl_y; // 마우스 좌표
std::random_device rd;
std::mt19937 gen(rd());
std::vector<TRIANGLE> triangles;
float rotationSpeed = 1.0; // 회전 속도 변수 
float totalTime = 0.0f;      // 애니메이션 전체 시간
float scaleSpeed = 2.0f;       // 크기 변경 속도 
float baseScale = 1.0f;        // 기본 크기
float scaleAmplitude = 0.25f;  // 크기 변화의 진폭
float previousScale = 1.0f; // 이전 프레임의 크기 저장 변수

GLuint vao; // --- 버텍스 배열 객체 이름
GLuint vbo[2];

GLvoid CreateTriangleShape1(float x, float y);
GLvoid CreateTriangleShape2(float x, float y);
GLvoid ChangePos();

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
    width = 800;
    height = 800;
    //--- 윈도우 생성하기
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("실습 14");
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
    glutIdleFunc(Animation);
    // glutTimerFunc(16, Timer, 0); // 타이머 실행시키고 시작
    glutMainLoop();
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

GLvoid DrawScene() //--- 콜백 함수: 그리기 콜백 함수
{
    GLfloat rColor, gColor, bColor;
    rColor = gColor = bColor = 1.0;
    glClearColor(rColor, gColor, bColor, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    // 폴리곤 모드 설정
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // 1사분면 그리기
    for (const auto& triangle : triangles) {
        glBindVertexArray(vao);

        // 버텍스 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle.vertices.size() * sizeof(float), triangle.vertices.data());

        // 색상 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle.colors.size() * sizeof(float), triangle.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }


    glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
    glViewport(0, 0, w, h);
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

GLvoid CreateTriangleShape1(float x, float y) {
    triangles.clear(); // 기존 도형들 삭제

    float outerSize = 0.5f; // 바깥쪽 사각형의 크기 (중심에서 꼭짓점까지의 거리)
    float innerSize = 0.3f; // 안쪽 사각형(빈 공간)의 크기

    float P_top[3] = { x, y + outerSize, 0.0f };
    float P_right[3] = { x + outerSize, y, 0.0f };
    float P_bottom[3] = { x, y - outerSize, 0.0f };
    float P_left[3] = { x - outerSize, y, 0.0f };

    float P_inner_tr[3] = { x + innerSize, y + innerSize, 0.0f }; // Top-Right
    float P_inner_tl[3] = { x - innerSize, y + innerSize, 0.0f }; // Top-Left
    float P_inner_bl[3] = { x - innerSize, y - innerSize, 0.0f }; // Bottom-Left
    float P_inner_br[3] = { x + innerSize, y - innerSize, 0.0f }; // Bottom-Right

    TRIANGLE top_triangle, right_triangle, bottom_triangle, left_triangle;

    // 1. 위쪽 삼각형
    top_triangle.vertices = {
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // 안쪽 왼쪽-위
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // 안쪽 오른쪽-위
        P_top[0],      P_top[1],      P_top[2]       // 바깥쪽 위
    };

    // 2. 오른쪽 삼각형
    right_triangle.vertices = {
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // 안쪽 오른쪽-위
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // 안쪽 오른쪽-아래
        P_right[0],    P_right[1],    P_right[2]     // 바깥쪽 오른쪽
    };

    // 3. 아래쪽 삼각형
    bottom_triangle.vertices = {
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // 안쪽 오른쪽-아래
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // 안쪽 왼쪽-아래
        P_bottom[0],   P_bottom[1],   P_bottom[2]    // 바깥쪽 아래
    };

    // 4. 왼쪽 삼각형
    left_triangle.vertices = {
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // 안쪽 왼쪽-아래
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // 안쪽 왼쪽-위
        P_left[0],     P_left[1],     P_left[2]      // 바깥쪽 왼쪽
    };

    float rcol = 0.0f;
    float gcol = 0.6f;
    float bcol = 0.6f;

    std::vector<float> common_colors = {
        rcol, gcol, bcol,
        rcol, gcol, bcol,
        rcol, gcol, bcol
    };

    top_triangle.colors = common_colors;
    right_triangle.colors = common_colors;
    bottom_triangle.colors = common_colors;
    left_triangle.colors = common_colors;

    triangles.push_back(top_triangle);
    triangles.push_back(right_triangle);
    triangles.push_back(bottom_triangle);
    triangles.push_back(left_triangle);
}

GLvoid CreateTriangleShape2(float x, float y) {
    triangles.clear(); // 기존 도형들 삭제

    float outerSize = 0.1f; // 바깥쪽 사각형의 크기 (중심에서 꼭짓점까지의 거리)
    float innerSize = 0.3f; // 안쪽 사각형(빈 공간)의 크기

    float P_top[3] = { x, y + outerSize, 0.0f };
    float P_right[3] = { x + outerSize, y, 0.0f };
    float P_bottom[3] = { x, y - outerSize, 0.0f };
    float P_left[3] = { x - outerSize, y, 0.0f };

    float P_inner_tr[3] = { x + innerSize, y + innerSize, 0.0f }; // Top-Right
    float P_inner_tl[3] = { x - innerSize, y + innerSize, 0.0f }; // Top-Left
    float P_inner_bl[3] = { x - innerSize, y - innerSize, 0.0f }; // Bottom-Left
    float P_inner_br[3] = { x + innerSize, y - innerSize, 0.0f }; // Bottom-Right

    TRIANGLE top_triangle, right_triangle, bottom_triangle, left_triangle;

    // 1. 위쪽 삼각형
    top_triangle.vertices = {
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // 안쪽 왼쪽-위
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // 안쪽 오른쪽-위
        P_top[0], P_top[1], P_top[2]       // 바깥쪽 위
    };

    // 2. 오른쪽 삼각형
    right_triangle.vertices = {
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // 안쪽 오른쪽-위
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // 안쪽 오른쪽-아래
        P_right[0], P_right[1], P_right[2]     // 바깥쪽 오른쪽
    };

    // 3. 아래쪽 삼각형
    bottom_triangle.vertices = {
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // 안쪽 오른쪽-아래
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // 안쪽 왼쪽-아래
        P_bottom[0], P_bottom[1], P_bottom[2]    // 바깥쪽 아래
    };

    // 4. 왼쪽 삼각형
    left_triangle.vertices = {
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // 안쪽 왼쪽-아래
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // 안쪽 왼쪽-위
        P_left[0], P_left[1], P_left[2]      // 바깥쪽 왼쪽
    };

    float rcol = 0.0f;
    float gcol = 0.6f;
    float bcol = 0.6f;

    std::vector<float> common_colors = {
        rcol, gcol, bcol,
        rcol, gcol, bcol,
        rcol, gcol, bcol
    };

    top_triangle.colors = common_colors;
    right_triangle.colors = common_colors;
    bottom_triangle.colors = common_colors;
    left_triangle.colors = common_colors;

    triangles.push_back(top_triangle);
    triangles.push_back(right_triangle);
    triangles.push_back(bottom_triangle);
    triangles.push_back(left_triangle);
}

GLvoid Keyboard(unsigned char key, int x, int y) {

    if (key == 'q' || key == 'Q') {
        std::cout << "종료" << std::endl;
        exit(0);
    }
    else if (key == '1') {
        TriangleShape = false;
        CreateTriangleShape1(0.0f, 0.0f);
    }
    else if (key == '2') {
        TriangleShape = true;
        CreateTriangleShape2(0.0f, 0.0f);
    }
    else if (key == 'c') {
        animation = true;
        TurnRight = true;
        TurnLeft = false;
    }
    else if (key == 't') {
        animation = true;
        TurnLeft = true;
        TurnRight = false;
    }
    else if (key == 's') {
        if (animation) {
            animation = false;
        }
        else {
            animation = true;
        }

    }
    else if (key == 'p') {
        ChangePos();
    }


    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
    gl_x = convertX(x, width);
    gl_y = convertY(y, height);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {

    }

    glutPostRedisplay();
}

GLvoid Motion(int x, int y) {
    glutPostRedisplay();
}

GLvoid Timer(int value) {
    glutPostRedisplay();
}

GLvoid Animation() {
    static int last_time = 0;
    int current_time = glutGet(GLUT_ELAPSED_TIME);

    if (last_time == 0) {
        last_time = current_time;
    }

    deltaTime = (current_time - last_time) / 1000.0f;
    last_time = current_time;

    if (animation)Update(deltaTime);

    glutPostRedisplay();
}

GLvoid Update(float deltaTime) {
    if (animation) {
        totalTime += deltaTime;

        float currentScale = baseScale + scaleAmplitude * sin(totalTime * scaleSpeed);

        float deltaScale = 1.0f;
        if (previousScale != 0) {
            deltaScale = currentScale / previousScale;
        }

        previousScale = currentScale;

        float deltaAngle = rotationSpeed * deltaTime;

        if (TurnRight) {
            for (auto& triangle : triangles) {
                for (int i = 0; i < triangle.vertices.size(); i += 3) {
                    float x = triangle.vertices[i];
                    float y = triangle.vertices[i + 1];

                    // 크기
                    float scaledX = x * deltaScale;
                    float scaledY = y * deltaScale;

                    // 회전
                    float rotatedX = scaledX * cos(-deltaAngle) - scaledY * sin(-deltaAngle);
                    float rotatedY = scaledX * sin(-deltaAngle) + scaledY * cos(-deltaAngle);

                    triangle.vertices[i] = rotatedX;
                    triangle.vertices[i + 1] = rotatedY;
                }
            }
        }
        else if (TurnLeft) {
            for (auto& triangle : triangles) {
                for (int i = 0; i < triangle.vertices.size(); i += 3) {
                    float x = triangle.vertices[i];
                    float y = triangle.vertices[i + 1];

                    // 크기
                    float scaledX = x * deltaScale;
                    float scaledY = y * deltaScale;

                    // 회전
                    float rotatedX = scaledX * cos(deltaAngle) - scaledY * sin(deltaAngle);
                    float rotatedY = scaledX * sin(deltaAngle) + scaledY * cos(deltaAngle);

                    triangle.vertices[i] = rotatedX;
                    triangle.vertices[i + 1] = rotatedY;
                }
            }
        }
    }

    glutPostRedisplay();
}


GLvoid ChangePos() {
    float moveX = 0.2;
    float moveY = 0.2;
    for (auto& triangle : triangles) {

        for (int i = 0; i < triangle.vertices.size(); i += 3) {
            triangle.vertices[i] += moveX;
            triangle.vertices[i + 1] += moveY;
        }
    }

}