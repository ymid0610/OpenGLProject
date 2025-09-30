#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <random>
#include <stdlib.h>
#include <stdio.h>

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

inline float convertX(int x, int windowWidth) {
    return (2.0f * x) / windowWidth - 1.0f;
}

inline float convertY(int y, int windowHeight) {
    return 1.0f - (2.0f * y) / windowHeight;
}

inline float convertWidth(int width, int windowWidth) {
    return (2.0f * width) / windowWidth;
}

inline float convertHeight(int height, int windowHeight) {
    return (2.0f * height) / windowHeight;
}

// OpenGL 좌표 -> 윈도우 좌표 변환 함수
inline int convertXBack(float x, int windowWidth) {
    return static_cast<int>(((x + 1.0f) * windowWidth) / 2.0f);
}
inline int convertYBack(float y, int windowHeight) {
    return static_cast<int>(((1.0f - y) * windowHeight) / 2.0f);
}
inline int convertWidthBack(float width, int windowWidth) {
    return static_cast<int>((width * windowWidth) / 2.0f);
}
inline int convertHeightBack(float height, int windowHeight) {
    return static_cast<int>((height * windowHeight) / 2.0f);
}

// 도형 구조체
struct Triangle {
    float x, y;
    std::vector<float> vertices;
    std::vector<float> colors;
};

// bool

int CheckQuadrant(float x, float y);

// 전역 변수
float gl_x, gl_y; // 마우스 좌표
float size = 0.1f; // 도형 크기
std::random_device rd;
std::mt19937 gen(rd());
bool wireframeMode = false; // 와이어프레임 모드 여부

std::vector<Triangle> triangles1; // 1 사분면
std::vector<Triangle> triangles2; // 2 사분면
std::vector<Triangle> triangles3; // 3 사분면
std::vector<Triangle> triangles4; // 4 사분면

int selectedShape = -1; // 선택된 도형 인덱스

GLuint vao; // --- 버텍스 배열 객체 이름
GLuint vbo[2];

// 도형 생성 함수들
void CreatePointShape(float x, float y);
void CreateLineShape(float x, float y);
void CreateTriangleShape(float x, float y);
void CreateRectangleShape(float x, float y);
//void UpdateBuffers();

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

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, -1.0f);    // 첫 번째 점
    glVertex2f(0.0f, 1.0f);    // 두 번째 점

    glVertex2f(-1.0f, 0.0f);    // 첫 번째 점
    glVertex2f(1.0f, 0.0f);    // 두 번째 점
    glEnd();

    glUseProgram(shaderProgramID);

    // 폴리곤 모드 설정
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // 1사분면 그리기
    for (const auto& triangle1 : triangles1) {
        glBindVertexArray(vao);

        // 버텍스 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.vertices.size() * sizeof(float), triangle1.vertices.data());

        // 색상 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.colors.size() * sizeof(float), triangle1.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // 2사분면 그리기
    for (const auto& triangle2 : triangles2) {
        glBindVertexArray(vao);
        // 버텍스 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle2.vertices.size() * sizeof(float), triangle2.vertices.data());
        // 색상 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle2.colors.size() * sizeof(float), triangle2.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // 3사분면 그리기
    for (const auto& triangle3 : triangles3) {
        glBindVertexArray(vao);
        // 버텍스 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle3.vertices.size() * sizeof(float), triangle3.vertices.data());
        // 색상 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle3.colors.size() * sizeof(float), triangle3.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);

    }

    // 4사분면 그리기
    for (const auto& triangle4 : triangles4) {
        glBindVertexArray(vao);
        // 버텍스 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle4.vertices.size() * sizeof(float), triangle4.vertices.data());
        // 색상 데이터 업데이트
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle4.colors.size() * sizeof(float), triangle4.colors.data());
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

void CreateTriangleShape(float x, float y) {
    Triangle triangle;
    triangle.x = x;
    triangle.y = y;
    std::uniform_real_distribution<double> dis(0.05, 0.1);
    std::uniform_real_distribution<double> col(0.0, 1.0);
    size = dis(gen);

    float rcol = col(gen);
    float gcol = col(gen);
    float bcol = col(gen);

    triangle.vertices = {
        x - size, y - size / 2, 0.0f,  // 왼쪽 하단
        x + size, y - size / 2, 0.0f,  // 오른쪽 하단
        x, y + size * 2, 0.0f          // 상단
    };

    triangle.colors = {
        rcol, gcol, bcol,
        rcol, gcol, bcol,
        rcol, gcol, bcol
    };

    if (CheckQuadrant(x, y) == 1) {
        triangles1.push_back(triangle);
    }
    else if (CheckQuadrant(x, y) == 2) {
        triangles2.push_back(triangle);
    }
    else if (CheckQuadrant(x, y) == 3) {
        triangles3.push_back(triangle);
    }
    else if (CheckQuadrant(x, y) == 4) {
        triangles4.push_back(triangle);
    }
}


GLvoid Keyboard(unsigned char key, int x, int y) {

    if (key == 'q' || key == 'Q') {
        std::cout << "종료" << std::endl;
        exit(0);
    }
    else if (key == 'c' || key == 'C') {
        triangles1.clear();
        triangles2.clear();
        triangles3.clear();
        triangles4.clear();
        std::cout << "모든 삼각형 삭제" << std::endl;
    }
    else if (key == 'a' || key == 'A') {
        wireframeMode = false;
        std::cout << "면 모드" << std::endl;
    }
    else if (key == 'b' || key == 'B') {
        wireframeMode = true;
        std::cout << "선 모드" << std::endl;
    }

    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
	gl_x = convertX(x, width);
	gl_y = convertY(y, height);

    int quadrant = CheckQuadrant(gl_x, gl_y);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (quadrant == 1) { // 1사분면
            if (triangles1.size() < 4) {
                CreateTriangleShape(gl_x, gl_y);
            }
            else if (triangles1.size() == 4) {
                triangles1.erase(triangles1.begin());
                CreateTriangleShape(gl_x, gl_y);
            }
        }
        else if (quadrant == 2) {
            if (triangles2.size() < 4) {
                CreateTriangleShape(gl_x, gl_y);
            }
            else if (triangles2.size() == 4) {
                triangles2.erase(triangles2.begin());
                CreateTriangleShape(gl_x, gl_y);
            }
        }
        else if (quadrant == 3) {
            if (triangles3.size() < 4) {
                CreateTriangleShape(gl_x, gl_y);
            }
            else if (triangles3.size() == 4) {
                triangles3.erase(triangles3.begin());
                CreateTriangleShape(gl_x, gl_y);
            }
        }
        else if (quadrant == 4) {
            if (triangles4.size() < 4) {
                CreateTriangleShape(gl_x, gl_y);
            }
            else if (triangles4.size() == 4) {
                triangles4.erase(triangles4.begin());
                CreateTriangleShape(gl_x, gl_y);
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (quadrant == 1 && triangles1.size() < 4) { // 1사분면
            CreateTriangleShape(gl_x, gl_y);
        }
        else if (quadrant == 2 && triangles2.size() < 4) {
            CreateTriangleShape(gl_x, gl_y);
        }
        else if (quadrant == 3 && triangles3.size() < 4) {
            CreateTriangleShape(gl_x, gl_y);
        }
        else if (quadrant == 4 && triangles4.size() < 4) {
            CreateTriangleShape(gl_x, gl_y);
        }

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
    glutPostRedisplay();
}

GLvoid Update(float deltaTime) {
    glutPostRedisplay();
}

int CheckQuadrant(float x, float y) {
    if (x >= 0.0 && y >= 0.0) return 1; // 1사분면
    else if (x < 0.0 && y >= 0.0) return 2; // 2사분면
    else if (x < 0.0 && y < 0.0) return 3; // 3사분면
    else return 4; // 4사분면  
}