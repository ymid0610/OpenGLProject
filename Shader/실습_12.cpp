#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include "ymshader.h"

void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void TimerFunction(int value);

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
void InitBuffer();
char* filetobuf(const char* file);
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint VAO, VBO_pos, EBO;
GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;

int v_count = 0;

enum ShapeType { LINE, TRIANGLE, RECTANGLE, PENTAGON };

struct Shape {
    std::vector<VERTEX> verts[4]; // 0:Line, 1:Triangle, 2:Rect, 3:Penta
    std::vector<unsigned int> indices[4];
    int idx; // 현재 도형 타입 (0~3)
    ShapeType type;
};

Shape centerShape;      // 중앙 도형
Shape shapes[4];        // 4사분면 도형
bool animating[5] = { false, false, false, false, false }; // 0:중앙, 1~4:사분면
ShapeType nextShape[5]; // 0:중앙, 1~4:사분면
bool flag_a = false;

void InitCenterShape() {
    centerShape.verts[0] = CreateLine(0.0f - 0.15f, 0.0f - 0.15f, 0.0f + 0.15f, 0.0f + 0.15f);
    centerShape.verts[1] = CreateTriangle(0.0f, 0.0f, 0.3f);
    centerShape.verts[2] = CreateRectangle(0.0f, 0.0f, 0.3f);
    centerShape.verts[3] = CreatePentagon(0.0f, 0.0f, 0.3f);
    for (int t = 0; t < 4; ++t) {
        centerShape.indices[t].clear();
        int vsize = centerShape.verts[t].size();
        if (t == 0) { // Line
            centerShape.indices[t].push_back(1);
            centerShape.indices[t].push_back(2);
        }
        else {
            for (int j = 1; j < vsize; ++j) {
                centerShape.indices[t].push_back(0);
                centerShape.indices[t].push_back(j);
                centerShape.indices[t].push_back((j % (vsize - 1)) + 1);
            }
        }
    }
    centerShape.idx = 1; // TRIANGLE
    centerShape.type = TRIANGLE;
}

void InitShapes() {
    float cx[4] = { -0.5f, 0.5f, -0.5f, 0.5f };
    float cy[4] = { 0.5f, 0.5f, -0.5f, -0.5f };
    float size = 0.3f;
    for (int i = 0; i < 4; ++i) {
        shapes[i].verts[0] = CreateLine(cx[i] - 0.15f, cy[i] - 0.15f, cx[i] + 0.15f, cy[i] + 0.15f);
        shapes[i].verts[1] = CreateTriangle(cx[i], cy[i], size);
        shapes[i].verts[2] = CreateRectangle(cx[i], cy[i], size);
        shapes[i].verts[3] = CreatePentagon(cx[i], cy[i], size);
        for (int t = 0; t < 4; ++t) {
            shapes[i].indices[t].clear();
            int vsize = shapes[i].verts[t].size();
            if (t == 0) { // Line
                shapes[i].indices[t].push_back(1);
                shapes[i].indices[t].push_back(2);
            }
            else {
                for (int j = 1; j < vsize; ++j) {
                    shapes[i].indices[t].push_back(0);
                    shapes[i].indices[t].push_back(j);
                    shapes[i].indices[t].push_back((j % (vsize - 1)) + 1);
                }
            }
        }
        shapes[i].idx = i; // 0:Line, 1:Triangle, 2:Rect, 3:Penta
        shapes[i].type = (ShapeType)i;
    }
}

void main(int argc, char** argv)
{
    InitCenterShape();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Example1");
    glewExperimental = GL_TRUE;
    glewInit();
    make_shaderProgram();
    InitBuffer();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutTimerFunc(16, TimerFunction, 1);
    glutMainLoop();
}

GLvoid drawScene()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    glBindVertexArray(VAO);

    if (!flag_a) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * centerShape.verts[centerShape.idx].size(), centerShape.verts[centerShape.idx].data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * centerShape.indices[centerShape.idx].size(), centerShape.indices[centerShape.idx].data(), GL_STATIC_DRAW);
        if (centerShape.idx == 0)
            glDrawElements(GL_LINES, centerShape.indices[0].size(), GL_UNSIGNED_INT, 0);
        else
            glDrawElements(GL_TRIANGLES, centerShape.indices[centerShape.idx].size(), GL_UNSIGNED_INT, 0);
    }
    else {
        for (int i = 0; i < 4; ++i) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
            glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX) * shapes[i].verts[shapes[i].idx].size(), shapes[i].verts[shapes[i].idx].data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * shapes[i].indices[shapes[i].idx].size(), shapes[i].indices[shapes[i].idx].data(), GL_STATIC_DRAW);
            if (shapes[i].idx == 0)
                glDrawElements(GL_LINES, shapes[i].indices[0].size(), GL_UNSIGNED_INT, 0);
            else
                glDrawElements(GL_TRIANGLES, shapes[i].indices[shapes[i].idx].size(), GL_UNSIGNED_INT, 0);
        }
    }
    glutSwapBuffers();
}

void Mouse(int button, int state, int x, int y) {
    glutPostRedisplay();
}
void Motion(int x, int y) {
    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        flag_a = !flag_a;
        if (!flag_a) InitCenterShape();
        else InitShapes();
        break;
    case 'l':
        if (!flag_a) { animating[0] = true; nextShape[0] = TRIANGLE; }
        else { animating[0] = true; nextShape[0] = TRIANGLE; }
        break;
    case 't':
        if (!flag_a) { animating[0] = true; nextShape[0] = RECTANGLE; }
        else { animating[1] = true; nextShape[1] = RECTANGLE; }
        break;
    case 'r':
        if (!flag_a) { animating[0] = true; nextShape[0] = PENTAGON; }
        else { animating[2] = true; nextShape[2] = PENTAGON; }
        break;
    case 'p':
        if (!flag_a) { animating[0] = true; nextShape[0] = LINE; }
        else { animating[3] = true; nextShape[3] = LINE; }
        break;
    }
    glutPostRedisplay();
}

void TimerFunction(int value) {
    if (!flag_a) {
        if (animating[0]) {
            std::vector<VERTEX>& current = centerShape.verts[centerShape.idx];
            std::vector<VERTEX> target = centerShape.verts[nextShape[0]];
            current = Move_Vertex(target, current);
            if (isCorrectMoved(target, current)) {
                centerShape.idx = nextShape[0];
                centerShape.type = nextShape[0];
                animating[0] = false;
            }
        }
    }
    else {
        for (int i = 0; i < 3; ++i) {
            if (animating[i]) {
                std::vector<VERTEX>& current = shapes[i].verts[shapes[i].idx];
                std::vector<VERTEX> target = shapes[i].verts[nextShape[i]];

                current = Move_Vertex(target, current);
                if (isCorrectMoved(target, current)) {
                    shapes[i].idx = nextShape[i];
                    shapes[i].type = nextShape[i];
                    animating[i] = false;
                }
            }
        }
		if (animating[3]) {
            if (isCorrectMoved(shapes[0].LineVertexs, finalVertex)) {
                shapes[0].type = LINE;
                flag_penta = false;
            else if (flag_penta && shapes[0].type == PENTAGON) {
                if (isCorrectMoved(shapes[0].LineVertexs, finalVertex)) {
                    shapes[0].type = LINE;
                    flag_penta = false;
                }
                else {
                    std::vector<VERTEX> temp;
                    temp = Move_LineVertex(shapes[0].LineVertexs, finalVertex);
                    finalVertex.clear();
                    for (int i = 0; i < temp.size(); i++) {
                        finalVertex.push_back(temp[i]);
                    }
                }
            }
		}

    }
    glutPostRedisplay();
    glutTimerFunc(16, TimerFunction, 1);
}

GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
void InitBuffer()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO_pos);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_pos);
    // drawScene에서 직접 glBufferData로 갱신함

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // drawScene에서 직접 glBufferData로 갱신함

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(VERTEX), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}
void make_vertexShaders()
{
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}
void make_shaderProgram()
{
    make_vertexShaders();
    make_fragmentShaders();
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgramID);
}
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
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}
