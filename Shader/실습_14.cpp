// unfirom ������ ��������� ����ؾ���
// ���� ��ġ�� �ٲ�� ��� ���������

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

//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

// ���� Ŭ����
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
bool animation = false; // �ִϸ��̼� ���� ����
bool TriangleShape = false;
bool wireframeMode = false; // ���̾������� ��� ����
bool TurnRight = false;
bool TurnLeft = false;

// ���� ����
float gl_x, gl_y; // ���콺 ��ǥ
std::random_device rd;
std::mt19937 gen(rd());
std::vector<TRIANGLE> triangles;
float rotationSpeed = 1.0; // ȸ�� �ӵ� ���� 
float totalTime = 0.0f;      // �ִϸ��̼� ��ü �ð�
float scaleSpeed = 2.0f;       // ũ�� ���� �ӵ� 
float baseScale = 1.0f;        // �⺻ ũ��
float scaleAmplitude = 0.25f;  // ũ�� ��ȭ�� ����
float previousScale = 1.0f; // ���� �������� ũ�� ���� ����

GLuint vao; // --- ���ؽ� �迭 ��ü �̸�
GLuint vbo[2];

GLvoid CreateTriangleShape1(float x, float y);
GLvoid CreateTriangleShape2(float x, float y);
GLvoid ChangePos();

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
    width = 800;
    height = 800;
    //--- ������ �����ϱ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("�ǽ� 14");
    //--- GLEW �ʱ�ȭ�ϱ�
    glewExperimental = GL_TRUE;
    glewInit();
    //--- ���̴� �о�ͼ� ���̴� ���α׷� �����: ����� �����Լ� ȣ��
    make_vertexShaders(); //--- ���ؽ� ���̴� �����
    make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
    shaderProgramID = make_shaderProgram();
    InitBuffer();

    //--- ���̴� ���α׷� �����
    glutDisplayFunc(DrawScene); //--- ��� �ݹ� �Լ�
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(Mouse);
    glutMotionFunc(Motion);
    glutIdleFunc(Animation);
    // glutTimerFunc(16, Timer, 0); // Ÿ�̸� �����Ű�� ����
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

GLvoid DrawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
    GLfloat rColor, gColor, bColor;
    rColor = gColor = bColor = 1.0;
    glClearColor(rColor, gColor, bColor, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    // ������ ��� ����
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // 1��и� �׸���
    for (const auto& triangle : triangles) {
        glBindVertexArray(vao);

        // ���ؽ� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle.vertices.size() * sizeof(float), triangle.vertices.data());

        // ���� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle.colors.size() * sizeof(float), triangle.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }


    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
    glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
    GLchar* vertexSource;
    //--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
    //--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
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
        std::cerr << "ERROR: vertex shader ������ ����" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders()
{
    GLchar* fragmentSource;
    //--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
    fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
        return;
    }
}

GLuint make_shaderProgram()
{
    GLuint shaderID;
    shaderID = glCreateProgram(); //--- ���̴� ���α׷� �����
    glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
    glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
    glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
    glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
    glDeleteShader(fragmentShader);
    GLint result;
    GLchar errorLog[512];
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
        return 0; // false ��� 0 ��ȯ
    }
    glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�

    return shaderID;
}

void InitBuffer() {
    glGenVertexArrays(1, &vao); //--- vao ���� �� �Ҵ�
    glBindVertexArray(vao); //--- vao ���ε�
    glGenBuffers(2, vbo); //--- vbo ���� �� �Ҵ�

    // ���ؽ� ���� (�ִ� 6�� ���� - �簢����)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- attribute 0: ���ؽ� ��ġ
    glEnableVertexAttribArray(0); //--- attribute 0 ��� ����

    // ���� ����
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0); //--- attribute 1: ���ؽ� ����
    glEnableVertexAttribArray(1); //--- attribute 1 ��� ����
}

GLvoid CreateTriangleShape1(float x, float y) {
    triangles.clear(); // ���� ������ ����

    float outerSize = 0.5f; // �ٱ��� �簢���� ũ�� (�߽ɿ��� ������������ �Ÿ�)
    float innerSize = 0.3f; // ���� �簢��(�� ����)�� ũ��

    float P_top[3] = { x, y + outerSize, 0.0f };
    float P_right[3] = { x + outerSize, y, 0.0f };
    float P_bottom[3] = { x, y - outerSize, 0.0f };
    float P_left[3] = { x - outerSize, y, 0.0f };

    float P_inner_tr[3] = { x + innerSize, y + innerSize, 0.0f }; // Top-Right
    float P_inner_tl[3] = { x - innerSize, y + innerSize, 0.0f }; // Top-Left
    float P_inner_bl[3] = { x - innerSize, y - innerSize, 0.0f }; // Bottom-Left
    float P_inner_br[3] = { x + innerSize, y - innerSize, 0.0f }; // Bottom-Right

    TRIANGLE top_triangle, right_triangle, bottom_triangle, left_triangle;

    // 1. ���� �ﰢ��
    top_triangle.vertices = {
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // ���� ����-��
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // ���� ������-��
        P_top[0],      P_top[1],      P_top[2]       // �ٱ��� ��
    };

    // 2. ������ �ﰢ��
    right_triangle.vertices = {
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // ���� ������-��
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // ���� ������-�Ʒ�
        P_right[0],    P_right[1],    P_right[2]     // �ٱ��� ������
    };

    // 3. �Ʒ��� �ﰢ��
    bottom_triangle.vertices = {
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // ���� ������-�Ʒ�
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // ���� ����-�Ʒ�
        P_bottom[0],   P_bottom[1],   P_bottom[2]    // �ٱ��� �Ʒ�
    };

    // 4. ���� �ﰢ��
    left_triangle.vertices = {
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // ���� ����-�Ʒ�
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // ���� ����-��
        P_left[0],     P_left[1],     P_left[2]      // �ٱ��� ����
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
    triangles.clear(); // ���� ������ ����

    float outerSize = 0.1f; // �ٱ��� �簢���� ũ�� (�߽ɿ��� ������������ �Ÿ�)
    float innerSize = 0.3f; // ���� �簢��(�� ����)�� ũ��

    float P_top[3] = { x, y + outerSize, 0.0f };
    float P_right[3] = { x + outerSize, y, 0.0f };
    float P_bottom[3] = { x, y - outerSize, 0.0f };
    float P_left[3] = { x - outerSize, y, 0.0f };

    float P_inner_tr[3] = { x + innerSize, y + innerSize, 0.0f }; // Top-Right
    float P_inner_tl[3] = { x - innerSize, y + innerSize, 0.0f }; // Top-Left
    float P_inner_bl[3] = { x - innerSize, y - innerSize, 0.0f }; // Bottom-Left
    float P_inner_br[3] = { x + innerSize, y - innerSize, 0.0f }; // Bottom-Right

    TRIANGLE top_triangle, right_triangle, bottom_triangle, left_triangle;

    // 1. ���� �ﰢ��
    top_triangle.vertices = {
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // ���� ����-��
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // ���� ������-��
        P_top[0], P_top[1], P_top[2]       // �ٱ��� ��
    };

    // 2. ������ �ﰢ��
    right_triangle.vertices = {
        P_inner_tr[0], P_inner_tr[1], P_inner_tr[2],   // ���� ������-��
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // ���� ������-�Ʒ�
        P_right[0], P_right[1], P_right[2]     // �ٱ��� ������
    };

    // 3. �Ʒ��� �ﰢ��
    bottom_triangle.vertices = {
        P_inner_br[0], P_inner_br[1], P_inner_br[2],   // ���� ������-�Ʒ�
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // ���� ����-�Ʒ�
        P_bottom[0], P_bottom[1], P_bottom[2]    // �ٱ��� �Ʒ�
    };

    // 4. ���� �ﰢ��
    left_triangle.vertices = {
        P_inner_bl[0], P_inner_bl[1], P_inner_bl[2],   // ���� ����-�Ʒ�
        P_inner_tl[0], P_inner_tl[1], P_inner_tl[2],   // ���� ����-��
        P_left[0], P_left[1], P_left[2]      // �ٱ��� ����
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
        std::cout << "����" << std::endl;
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

                    // ũ��
                    float scaledX = x * deltaScale;
                    float scaledY = y * deltaScale;

                    // ȸ��
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

                    // ũ��
                    float scaledX = x * deltaScale;
                    float scaledY = y * deltaScale;

                    // ȸ��
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