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

//--- �ʿ��� ���� ����
GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

// ���� ����ü
struct Triangle {
    float x, y;
    std::vector<float> vertices;
    std::vector<float> colors;
};

// ���� ����
float gl_x, gl_y; // ���콺 ��ǥ
float size = 0.1f; // ���� ũ��
std::random_device rd;
std::mt19937 gen(rd());

static std::vector<Triangle> triangles;

GLuint vao; // --- ���ؽ� �迭 ��ü �̸�
GLuint vbo[2];

void CreateTriangleShape(float x, float y);

class SPIROR {
public:
	float CenterX1, CenterY1; // �� �����̷� �߽� ��ǥ1
	float CenterX2, CenterY2; // �� �����̷� �߽� ��ǥ2
    float x, y;
	float angle = 0.0f; // ���� ����
	float radius = 0.0f; // ���� ������
	float angle_step = 5.0f; // ���� ������ (�� ����) 720 / 5 = 144
	float radius_step = 0.001f; // ������ ������
	int direction = 0; 
	bool spiral_done = false;

    std::vector<float> points; // �� �����̷��� �� �迭

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
    glutTimerFunc(16, Timer, 0); // Ÿ�̸� �����Ű�� ����
    glutMainLoop();
}
GLvoid DrawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	glClearColor(rc, gc, bc, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    if (flag) {
        // ��� �����̷� ������ ������ �׸�
        for (const auto& spiror : spirors) {
            if (spiror.points.size() < 6) continue; // �ּ� 2�� �̻�
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, spiror.points.size() * sizeof(float), spiror.points.data());

            // ����: �Ķ���
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

            // ���ؽ� ������ ������Ʈ
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.vertices.size() * sizeof(float), triangle1.vertices.data());

            // ���� ������ ������Ʈ
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

            glBufferSubData(GL_ARRAY_BUFFER, 0, triangle1.colors.size() * sizeof(float), triangle1.colors.data());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
    glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
    glViewport(0, 0, w, h);
}
GLvoid Keyboard(unsigned char key, int x, int y) {

    std::uniform_real_distribution<float> loc(-1.0f, 1.0f);
    if (key == 'q' || key == 'Q') {
        std::cout << "����" << std::endl;
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
        x - size, y - size / 2, 0.0f,  // ���� �ϴ�
        x + size, y - size / 2, 0.0f,  // ������ �ϴ�
        x, y + size * 2, 0.0f          // ���
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

        // ���ؽ� ���� (�����̷���: ����� ũ��)
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, 3000 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // ���� ����
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, 3000 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }
    else {
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