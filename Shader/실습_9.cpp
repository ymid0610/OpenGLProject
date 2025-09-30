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

//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

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

// OpenGL ��ǥ -> ������ ��ǥ ��ȯ �Լ�
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

// ���� ����ü
struct Triangle {
    float x, y;
    std::vector<float> vertices;
    std::vector<float> colors;
};

// bool

int CheckQuadrant(float x, float y);

// ���� ����
float gl_x, gl_y; // ���콺 ��ǥ
float size = 0.1f; // ���� ũ��
std::random_device rd;
std::mt19937 gen(rd());
bool wireframeMode = false; // ���̾������� ��� ����

std::vector<Triangle> triangles1; // 1 ��и�
std::vector<Triangle> triangles2; // 2 ��и�
std::vector<Triangle> triangles3; // 3 ��и�
std::vector<Triangle> triangles4; // 4 ��и�

int selectedShape = -1; // ���õ� ���� �ε���

GLuint vao; // --- ���ؽ� �迭 ��ü �̸�
GLuint vbo[2];

// ���� ���� �Լ���
void CreatePointShape(float x, float y);
void CreateLineShape(float x, float y);
void CreateTriangleShape(float x, float y);
void CreateRectangleShape(float x, float y);
//void UpdateBuffers();

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
    width = 800;
    height = 800;
    //--- ������ �����ϱ�
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("�ǽ� 8");
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

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, -1.0f);    // ù ��° ��
    glVertex2f(0.0f, 1.0f);    // �� ��° ��

    glVertex2f(-1.0f, 0.0f);    // ù ��° ��
    glVertex2f(1.0f, 0.0f);    // �� ��° ��
    glEnd();

    glUseProgram(shaderProgramID);

    // ������ ��� ����
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // 1��и� �׸���
    for (const auto& triangle1 : triangles1) {
        glBindVertexArray(vao);

        // ���ؽ� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.vertices.size() * sizeof(float), triangle1.vertices.data());

        // ���� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.colors.size() * sizeof(float), triangle1.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // 2��и� �׸���
    for (const auto& triangle2 : triangles2) {
        glBindVertexArray(vao);
        // ���ؽ� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle2.vertices.size() * sizeof(float), triangle2.vertices.data());
        // ���� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle2.colors.size() * sizeof(float), triangle2.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    // 3��и� �׸���
    for (const auto& triangle3 : triangles3) {
        glBindVertexArray(vao);
        // ���ؽ� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle3.vertices.size() * sizeof(float), triangle3.vertices.data());
        // ���� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle3.colors.size() * sizeof(float), triangle3.colors.data());
        glDrawArrays(GL_TRIANGLES, 0, 3);

    }

    // 4��и� �׸���
    for (const auto& triangle4 : triangles4) {
        glBindVertexArray(vao);
        // ���ؽ� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle4.vertices.size() * sizeof(float), triangle4.vertices.data());
        // ���� ������ ������Ʈ
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle4.colors.size() * sizeof(float), triangle4.colors.data());
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
        x - size, y - size / 2, 0.0f,  // ���� �ϴ�
        x + size, y - size / 2, 0.0f,  // ������ �ϴ�
        x, y + size * 2, 0.0f          // ���
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
        std::cout << "����" << std::endl;
        exit(0);
    }
    else if (key == 'c' || key == 'C') {
        triangles1.clear();
        triangles2.clear();
        triangles3.clear();
        triangles4.clear();
        std::cout << "��� �ﰢ�� ����" << std::endl;
    }
    else if (key == 'a' || key == 'A') {
        wireframeMode = false;
        std::cout << "�� ���" << std::endl;
    }
    else if (key == 'b' || key == 'B') {
        wireframeMode = true;
        std::cout << "�� ���" << std::endl;
    }

    glutPostRedisplay();
}

GLvoid Mouse(int button, int state, int x, int y) {
	gl_x = convertX(x, width);
	gl_y = convertY(y, height);

    int quadrant = CheckQuadrant(gl_x, gl_y);

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (quadrant == 1) { // 1��и�
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
        if (quadrant == 1 && triangles1.size() < 4) { // 1��и�
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
    if (x >= 0.0 && y >= 0.0) return 1; // 1��и�
    else if (x < 0.0 && y >= 0.0) return 2; // 2��и�
    else if (x < 0.0 && y < 0.0) return 3; // 3��и�
    else return 4; // 4��и�  
}