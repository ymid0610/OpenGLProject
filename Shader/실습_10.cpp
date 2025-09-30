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

enum Move_type { STOP, CROSS, ZIGZAG, RECT_SPIROR, CIR_SPIROR};

// ���� ����ü
class Triangle {
public:
    float x, y, size;
    std::vector<float> vertices;
    std::vector<float> colors;
	float dix = 1.0f, diy = -1.0f; // �̵� ���� ����
	Move_type move_type;

    // �簢 �����̷� ���� ����
    int spiral_phase = 4; // 0:��, 1:��, 2:��, 3:��
    float spiral_dist = 2.0f; // ���� �̵� �Ÿ�
    float spiral_step = 0.02f; // �� ���� �̵��ϴ� �Ÿ�
    float spiral_remain = 2.0f; // ���� �Ÿ�
    float spiral_shrink = 0.2f; // �� ���� �� ������ �پ��� �Ÿ�
	bool spiral_in = true; // �������� ������ ����

    // �� �����̷� ���� ����
	float CenterX, CenterY; // �� �����̷� �߽� ��ǥ
	float angle = 0.0f; // ���� ����
	float radius = 0.0f; // ���� ������
	float angle_step = 5.0f; // ���� ������ (�� ����)
	float radius_step = 0.001f; // ������ ������
    bool spiral_out = true;

    void move_cross() {
        // �߽� ��ǥ �̵�
        x += dix * 0.01f;
        y += diy * 0.01f;

        // x�� �� �浹 (size ���)
        if (x + size > 1.0f || x - size < -1.0f) {
            dix = -dix;
            // ���� ���̱�
            if (x + size > 1.0f) x = 1.0f - size;
            if (x - size < -1.0f) x = -1.0f + size;
        }
        // y�� �� �浹 (size ���)
        if (y + size * 2 > 1.0f || y - size / 2 < -1.0f) {
            diy = -diy;
            // ���� ���̱�
            if (y + size * 2 > 1.0f) y = 1.0f - size * 2;
            if (y - size / 2 < -1.0f) y = -1.0f + size / 2;
        }
        // ������ ����
        remake_vertices();
    }
    void move_zigzag() {
        // �߽� ��ǥ �̵�
        x += dix * 0.01f;

        // x�� �� ���� (size�� ���)
        if (x + size > 1.0f || x - size < -1.0f) {
            dix = -dix; // x ���� ����
            // y������ �� ĭ �̵� (size�� ���)
            float new_y = y + diy * 0.1f;
            // y�� �� ����
            if (new_y + size * 2 > 1.0f || new_y - size / 2 < -1.0f) {
                diy = -diy; // y ���� ����
            }
            else {
                y = new_y;
            }
        }
        else {
            // y�� �� ���� (x�� ���� ���� �ʾ��� ����)
            if (y + size * 2 > 1.0f) {
                diy = -1.0f;
                y -= 0.1f;
            }
            if (y - size / 2 < -1.0f) {
                diy = 1.0f;
                y += 0.1f;
            }
        }

        // ������ ����
        remake_vertices();
    }
    void move_rect_spiror() {
        if (spiral_phase == 4) { // �ʱ�ȭ
            spiral_phase = 0; // ���� �̵����� ����
            spiral_dist = 2.0f; // ���� �̵� �Ÿ�
            spiral_remain = spiral_dist;
            size = return_size_from_vertices();
            spiral_in = true;
        }

        // ���� �� ���� �ٱ������� ��ȯ
        if (spiral_in && spiral_dist <= 0.0f) {
            spiral_in = false;
            spiral_dist = spiral_shrink; // �ٱ��� ������ �ּ� �Ÿ�����
            spiral_remain = spiral_dist;
        }

        // �ٱ������� �� �� spiral_dist�� ���� �ø�
        if (!spiral_in && spiral_remain <= 0.0f) {
            spiral_dist += spiral_shrink; // �ٱ������� �� ������ �Ÿ� ����
            spiral_remain = spiral_dist;
        }

        switch (spiral_phase) {
        case 0: // ���� �̵�
            if (spiral_in) {
                x -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 1;
                    spiral_remain = spiral_dist;
                }
                if (x - size < -1.0f) { // ���� �� �浹
                    spiral_phase = 1; // �Ʒ��� ���� ��ȯ
                    spiral_dist = 2.0f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    x = -1.0f + size; // ���� ���߱�
                }
            }
            else {
                x -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 1;
                }
				if (x - size < -1.0f) { // ���� �� �浹 �� 
					spiral_in = true; // �ٽ� �������� ����
					spiral_phase = 1;
					spiral_dist = 2.0f; // �Ÿ� �ʱ�ȭ
					spiral_remain = spiral_dist;
					x = -1.0f + size; // ���� ���߱�
				}
            }
            break;
        case 1: // �Ʒ� �̵�
            if (spiral_in) {
                y -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 2;
                    spiral_dist -= spiral_shrink; // �Ÿ� ���̱�
                    spiral_remain = spiral_dist;
                }
                if (y - size / 2 < -1.0f) { // �Ʒ� �� �浹
                    spiral_phase = 2; // ���������� ���� ��ȯ
                    spiral_dist = 1.8f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    y = -1.0f + size / 2; // ���� ���߱�
                }
            }
            else {
                y -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 2;
                }
                if (y - size / 2 < -1.0f) { // �Ʒ� �� �浹 ��
                    spiral_in = true; // �ٽ� �������� ����
                    spiral_phase = 2;
                    spiral_dist = 2.0f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    y = -1.0f + size / 2; // ���� ���߱�
                }
            }
            break;
        case 2: // ���� �̵�
            if (spiral_in) {
                x += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 3;
                    spiral_remain = spiral_dist;
                }
                if (x + size > 1.0f) { // ������ �� �浹
                    spiral_phase = 3; // ���� ���� ��ȯ
                    spiral_dist = 1.8f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    x = 1.0f - size; // ���� ���߱�
                }
            }
            else {
                x += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 3;
                }
				if (x + size > 1.0f) { // ������ �� �浹 ��
					spiral_in = true; // �ٽ� �������� ����
					spiral_phase = 3;
					spiral_dist = 2.0f; // �Ÿ� �ʱ�ȭ
					spiral_remain = spiral_dist;
					x = 1.0f - size; // ���� ���߱�
				}
            }
            break;
        case 3: // �� �̵�
            if (spiral_in) {
                y += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 0;
                    spiral_dist -= spiral_shrink; // �Ÿ� ���̱�
                    spiral_remain = spiral_dist;
                }
                if (y + size * 2 >= 1.0f) { // �� �� �浹
                    spiral_phase = 0; // �������� ���� ��ȯ
                    spiral_dist = 1.6f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    y = 1.0f - size * 2; // ���� ���߱�
                }
            }
            else {
                y += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 0;
                }
                if (y + size * 2 >= 1.0f) { // �� �� �浹 ��
                    spiral_in = true; // �ٽ� �������� ����
                    spiral_phase = 0;
                    spiral_dist = 2.0f; // �Ÿ� �ʱ�ȭ
                    spiral_remain = spiral_dist;
                    y = 1.0f - size * 2; // ���� ���߱�
                }
            }
            break;
        }
        // ������ ����
        vertices = {
            x - size, y - size / 2, 0.0f,
            x + size, y - size / 2, 0.0f,
            x, y + size * 2, 0.0f
        };
    }
    void move_cir_spiror() {
        if (spiral_out) {
            x = CenterX + radius * cos(angle * 3.14159f / 180.0f);
            y = CenterY + radius * sin(angle * 3.14159f / 180.0f);
            angle += angle_step;
            radius += radius_step;
			if (x + size > 1.0f || x - size < -1.0f || y + size * 2 > 1.0f || y - size / 2 < -1.0f) {
				spiral_out = false;
			}
        }
        else {
            x = CenterX + radius * cos(angle * 3.14159f / 180.0f);
            y = CenterY + radius * sin(angle * 3.14159f / 180.0f);
			angle -= angle_step;
			radius -= radius_step;
            if (radius <= 0.0f) {
                spiral_out = true;
				angle = 0.0f;
				radius = 0.0f;
            }
        }
        remake_vertices();
    }
    float return_size_from_vertices() const {
        if (vertices.size() < 6) return 0.0f;
        // ���� �ϴ�(x1), ������ �ϴ�(x2)
        float x1 = vertices[0];
        float x2 = vertices[3];
        // �غ� ���� = x2 - x1 = 2 * size
        return std::abs(x2 - x1) / 2.0f;
    }
	void remake_vertices() {
		vertices = {
			x - size, y - size / 2, 0.0f,
			x + size, y - size / 2, 0.0f,
			x, y + size * 2, 0.0f
		};
	}
    void remake_xy() {
		x = (vertices[0] + vertices[3]) / 2.0f; // x ��ǥ�� �غ��� �߰�
		y = (vertices[1] + vertices[7]) / 2.0f; // y ��ǥ�� �غ��� �������� �߰�
    }
    void remake_Center() {
        CenterX = x;
        CenterY = y;
    }
};

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

void CreateTriangleShape(float x, float y);

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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
    else if (key == '1') {
		for (auto& tri : triangles1) {
			tri.move_type = CROSS;
		}
        for (auto& tri : triangles2) {
            tri.move_type = CROSS;
        }
		for (auto& tri : triangles3) {
			tri.move_type = CROSS;
		}
        for (auto& tri : triangles4) {
            tri.move_type = CROSS;
        }
    }
	else if (key == '2') {
		for (auto& tri : triangles1) {
			tri.move_type = ZIGZAG;
		}
		for (auto& tri : triangles2) {
			tri.move_type = ZIGZAG;
		}
		for (auto& tri : triangles3) {
			tri.move_type = ZIGZAG;
		}
        for (auto& tri : triangles4) {
            tri.move_type = ZIGZAG;
        }
	}
	else if (key == '3') {
		for (auto& tri : triangles1) {
			tri.move_type = RECT_SPIROR;
		}
		for (auto& tri : triangles2) {
			tri.move_type = RECT_SPIROR;
		}
		for (auto& tri : triangles3) {
			tri.move_type = RECT_SPIROR;
		}
		for (auto& tri : triangles4) {
			tri.move_type = RECT_SPIROR;
		}
	}
	else if (key == '4') {
		for (auto& tri : triangles1) {
            tri.remake_Center();
			tri.move_type = CIR_SPIROR;
		}
		for (auto& tri : triangles2) {
			tri.remake_Center();
			tri.move_type = CIR_SPIROR;
		}
		for (auto& tri : triangles3) {
			tri.remake_Center();
			tri.move_type = CIR_SPIROR;
		}
		for (auto& tri : triangles4) {
			tri.remake_Center();
			tri.move_type = CIR_SPIROR;
		}
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
    for (int i = 0; i < triangles1.size(); i++) {
		if (triangles1[i].move_type == CROSS)
			triangles1[i].move_cross();
		else if (triangles1[i].move_type == ZIGZAG)
			triangles1[i].move_zigzag();
		else if (triangles1[i].move_type == RECT_SPIROR)
            triangles1[i].move_rect_spiror();
		else if (triangles1[i].move_type == CIR_SPIROR)
			triangles1[i].move_cir_spiror();
    }
	for (int i = 0; i < triangles2.size(); i++) {
		if (triangles2[i].move_type == CROSS)
			triangles2[i].move_cross();
		else if (triangles2[i].move_type == ZIGZAG)
			triangles2[i].move_zigzag();
		else if (triangles2[i].move_type == RECT_SPIROR)
			triangles2[i].move_rect_spiror();
		else if (triangles2[i].move_type == CIR_SPIROR)
			triangles2[i].move_cir_spiror();
	}
	for (int i = 0; i < triangles3.size(); i++) {
		if (triangles3[i].move_type == CROSS)
			triangles3[i].move_cross();
		else if (triangles3[i].move_type == ZIGZAG)
			triangles3[i].move_zigzag();
		else if (triangles3[i].move_type == RECT_SPIROR)
			triangles3[i].move_rect_spiror();
		else if (triangles3[i].move_type == CIR_SPIROR)
			triangles3[i].move_cir_spiror();
	}
	for (int i = 0; i < triangles4.size(); i++) {
		if (triangles4[i].move_type == CROSS)
			triangles4[i].move_cross();
		else if (triangles4[i].move_type == ZIGZAG)
			triangles4[i].move_zigzag();
		else if (triangles4[i].move_type == RECT_SPIROR)
			triangles4[i].move_rect_spiror();
		else if (triangles4[i].move_type == CIR_SPIROR)
			triangles4[i].move_cir_spiror();
	}
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

void CreateTriangleShape(float x, float y) {
    Triangle triangle;
    triangle.x = x;
    triangle.y = y;
    std::uniform_real_distribution<double> dis(0.05, 0.1);
    std::uniform_real_distribution<double> col(0.0, 1.0);
    size = dis(gen);

	triangle.size = size;

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