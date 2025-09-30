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

enum Move_type { STOP, CROSS, ZIGZAG, RECT_SPIROR, CIR_SPIROR};

// 도형 구조체
class Triangle {
public:
    float x, y, size;
    std::vector<float> vertices;
    std::vector<float> colors;
	float dix = 1.0f, diy = -1.0f; // 이동 방향 벡터
	Move_type move_type;

    // 사각 스파이럴 관련 변수
    int spiral_phase = 4; // 0:좌, 1:하, 2:우, 3:상
    float spiral_dist = 2.0f; // 최초 이동 거리
    float spiral_step = 0.02f; // 한 번에 이동하는 거리
    float spiral_remain = 2.0f; // 남은 거리
    float spiral_shrink = 0.2f; // 한 바퀴 돌 때마다 줄어드는 거리
	bool spiral_in = true; // 안쪽으로 도는지 여부

    // 원 스파이럴 관련 변수
	float CenterX, CenterY; // 원 스파이럴 중심 좌표
	float angle = 0.0f; // 현재 각도
	float radius = 0.0f; // 현재 반지름
	float angle_step = 5.0f; // 각도 증가량 (도 단위)
	float radius_step = 0.001f; // 반지름 증가량
    bool spiral_out = true;

    void move_cross() {
        // 중심 좌표 이동
        x += dix * 0.01f;
        y += diy * 0.01f;

        // x축 벽 충돌 (size 고려)
        if (x + size > 1.0f || x - size < -1.0f) {
            dix = -dix;
            // 벽에 붙이기
            if (x + size > 1.0f) x = 1.0f - size;
            if (x - size < -1.0f) x = -1.0f + size;
        }
        // y축 벽 충돌 (size 고려)
        if (y + size * 2 > 1.0f || y - size / 2 < -1.0f) {
            diy = -diy;
            // 벽에 붙이기
            if (y + size * 2 > 1.0f) y = 1.0f - size * 2;
            if (y - size / 2 < -1.0f) y = -1.0f + size / 2;
        }
        // 꼭짓점 재계산
        remake_vertices();
    }
    void move_zigzag() {
        // 중심 좌표 이동
        x += dix * 0.01f;

        // x축 벽 판정 (size를 고려)
        if (x + size > 1.0f || x - size < -1.0f) {
            dix = -dix; // x 방향 반전
            // y축으로 한 칸 이동 (size를 고려)
            float new_y = y + diy * 0.1f;
            // y축 벽 판정
            if (new_y + size * 2 > 1.0f || new_y - size / 2 < -1.0f) {
                diy = -diy; // y 방향 반전
            }
            else {
                y = new_y;
            }
        }
        else {
            // y축 벽 판정 (x축 벽에 닿지 않았을 때만)
            if (y + size * 2 > 1.0f) {
                diy = -1.0f;
                y -= 0.1f;
            }
            if (y - size / 2 < -1.0f) {
                diy = 1.0f;
                y += 0.1f;
            }
        }

        // 꼭짓점 재계산
        remake_vertices();
    }
    void move_rect_spiror() {
        if (spiral_phase == 4) { // 초기화
            spiral_phase = 0; // 좌측 이동부터 시작
            spiral_dist = 2.0f; // 최초 이동 거리
            spiral_remain = spiral_dist;
            size = return_size_from_vertices();
            spiral_in = true;
        }

        // 안쪽 다 돌면 바깥쪽으로 전환
        if (spiral_in && spiral_dist <= 0.0f) {
            spiral_in = false;
            spiral_dist = spiral_shrink; // 바깥쪽 시작은 최소 거리부터
            spiral_remain = spiral_dist;
        }

        // 바깥쪽으로 돌 때 spiral_dist를 점점 늘림
        if (!spiral_in && spiral_remain <= 0.0f) {
            spiral_dist += spiral_shrink; // 바깥쪽으로 돌 때마다 거리 증가
            spiral_remain = spiral_dist;
        }

        switch (spiral_phase) {
        case 0: // 좌측 이동
            if (spiral_in) {
                x -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 1;
                    spiral_remain = spiral_dist;
                }
                if (x - size < -1.0f) { // 왼쪽 벽 충돌
                    spiral_phase = 1; // 아래로 방향 전환
                    spiral_dist = 2.0f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    x = -1.0f + size; // 벽에 맞추기
                }
            }
            else {
                x -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 1;
                }
				if (x - size < -1.0f) { // 왼쪽 벽 충돌 시 
					spiral_in = true; // 다시 안쪽으로 돌기
					spiral_phase = 1;
					spiral_dist = 2.0f; // 거리 초기화
					spiral_remain = spiral_dist;
					x = -1.0f + size; // 벽에 맞추기
				}
            }
            break;
        case 1: // 아래 이동
            if (spiral_in) {
                y -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 2;
                    spiral_dist -= spiral_shrink; // 거리 줄이기
                    spiral_remain = spiral_dist;
                }
                if (y - size / 2 < -1.0f) { // 아래 벽 충돌
                    spiral_phase = 2; // 오른쪽으로 방향 전환
                    spiral_dist = 1.8f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    y = -1.0f + size / 2; // 벽에 맞추기
                }
            }
            else {
                y -= spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 2;
                }
                if (y - size / 2 < -1.0f) { // 아래 벽 충돌 시
                    spiral_in = true; // 다시 안쪽으로 돌기
                    spiral_phase = 2;
                    spiral_dist = 2.0f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    y = -1.0f + size / 2; // 벽에 맞추기
                }
            }
            break;
        case 2: // 우측 이동
            if (spiral_in) {
                x += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 3;
                    spiral_remain = spiral_dist;
                }
                if (x + size > 1.0f) { // 오른쪽 벽 충돌
                    spiral_phase = 3; // 위로 방향 전환
                    spiral_dist = 1.8f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    x = 1.0f - size; // 벽에 맞추기
                }
            }
            else {
                x += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 3;
                }
				if (x + size > 1.0f) { // 오른쪽 벽 충돌 시
					spiral_in = true; // 다시 안쪽으로 돌기
					spiral_phase = 3;
					spiral_dist = 2.0f; // 거리 초기화
					spiral_remain = spiral_dist;
					x = 1.0f - size; // 벽에 맞추기
				}
            }
            break;
        case 3: // 위 이동
            if (spiral_in) {
                y += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 0;
                    spiral_dist -= spiral_shrink; // 거리 줄이기
                    spiral_remain = spiral_dist;
                }
                if (y + size * 2 >= 1.0f) { // 위 벽 충돌
                    spiral_phase = 0; // 왼쪽으로 방향 전환
                    spiral_dist = 1.6f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    y = 1.0f - size * 2; // 벽에 맞추기
                }
            }
            else {
                y += spiral_step;
                spiral_remain -= spiral_step;
                if (spiral_remain <= 0.0f) {
                    spiral_phase = 0;
                }
                if (y + size * 2 >= 1.0f) { // 위 벽 충돌 시
                    spiral_in = true; // 다시 안쪽으로 돌기
                    spiral_phase = 0;
                    spiral_dist = 2.0f; // 거리 초기화
                    spiral_remain = spiral_dist;
                    y = 1.0f - size * 2; // 벽에 맞추기
                }
            }
            break;
        }
        // 꼭짓점 재계산
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
        // 왼쪽 하단(x1), 오른쪽 하단(x2)
        float x1 = vertices[0];
        float x2 = vertices[3];
        // 밑변 길이 = x2 - x1 = 2 * size
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
		x = (vertices[0] + vertices[3]) / 2.0f; // x 좌표는 밑변의 중간
		y = (vertices[1] + vertices[7]) / 2.0f; // y 좌표는 밑변과 꼭짓점의 중간
    }
    void remake_Center() {
        CenterX = x;
        CenterY = y;
    }
};

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

void CreateTriangleShape(float x, float y);

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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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