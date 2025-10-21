#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>
#include <set>

#define pi 3.14159265358979323846

struct VERTEX {
	float x, y, z;    // 위치
	float r, g, b;    // 색상
	int id;          // 정점 번호
};

extern int v_count;
static float red = 0.0f, green = 0.0f, blue = 1.0f;

GLfloat ConvertMouseToOGL_x(int x, int windowWidth) {
	return (2.0f * x) / windowWidth - 1.0f;
}
GLfloat ConvertMouseToOGL_y(int y, int windowHeight) {
	return 1.0f - (2.0f * y) / windowHeight;
}
void RandomColor() {
	red = rand() % 100 / 100.0f;
	green = rand() % 100 / 100.0f;
	blue = rand() % 100 / 100.0f;
}
std::vector<VERTEX> CreateTriangle(GLfloat x, GLfloat y, float size) {
	std::vector<VERTEX> TEMP;
	RandomColor();
	TEMP.push_back({ x, y, 0.0f, red, green, blue, v_count++ });

	float r = size / sqrt(3);
	float angle0 = pi / 2;
	float angle1 = angle0 + 2 * pi / 3;
	float angle2 = angle0 + 4 * pi / 3;
	TEMP.push_back({ x + r * cos(angle0), y + r * sin(angle0), 0.0f, red, green, blue, v_count++});
	TEMP.push_back({ x + r * cos(angle1), y + r * sin(angle1), 0.0f, red, green, blue, v_count++});					  
	TEMP.push_back({ x + r * cos(angle2), y + r * sin(angle2), 0.0f, red, green, blue, v_count++});
	TEMP.push_back({ x + r * cos(angle0), y + r * sin(angle0), 0.0f, red, green, blue, v_count++ });
	TEMP.push_back({ x + r * cos(angle1), y + r * sin(angle1), 0.0f, red, green, blue, v_count++ });

	return TEMP;

}
std::vector<VERTEX> CreateRectangle(GLfloat x, GLfloat y, float size) {
	std::vector<VERTEX> TEMP;
	RandomColor();
	TEMP.push_back({ x, y, 0.0f, red, green, blue, v_count++ });
	float half = size / 2;
	TEMP.push_back({ x - half, y - half, 0.0f, red, green, blue, v_count++ });
	TEMP.push_back({ x + half, y - half, 0.0f, red, green, blue, v_count++ });
	TEMP.push_back({ x + half, y + half, 0.0f, red, green, blue, v_count++ });
	TEMP.push_back({ x - half, y + half, 0.0f, red, green, blue, v_count++ });
	TEMP.push_back({ x - half, y - half, 0.0f, red, green, blue, v_count++ });
	return TEMP;
}
std::vector<VERTEX> CreatePentagon(GLfloat x, GLfloat y, float size) {
	std::vector<VERTEX> TEMP;
	float angle0 = -pi / 2; // 위쪽부터 시작
	TEMP.push_back({ x, y, 0.0f, red, green, blue, v_count++ });
	for (int i = 0; i < 5; ++i) {
		float angle = angle0 + i * 2 * pi / 5;
		float vx = x + size * cos(angle);
		float vy = y + size * sin(angle);
		TEMP.push_back({ vx, vy, 0.0f, red, green, blue, v_count++ });
	}
	return TEMP;
}

std::vector<VERTEX> Move_Vertex(std::vector<VERTEX> vertex1, std::vector<VERTEX> vertex2) {
	for (int i = 0; i < vertex1.size(); i++) {
		if (vertex1[i].x != vertex2[i].x) {
			vertex2[i].x += (vertex1[i].x - vertex2[i].x) / 100;
		}
		if (vertex1[i].y != vertex2[i].y) {
			vertex2[i].y += (vertex1[i].y - vertex2[i].y) / 100;
		}
	}
	return vertex2;
}

bool isCorrectMoved(std::vector<VERTEX> vertex1, std::vector<VERTEX> vertex2) {
	for (int i = 0; i < vertex1.size(); i++) {
		if (abs(vertex1[i].x - vertex2[i].x) > 0.001f || abs(vertex1[i].y - vertex2[i].y) > 0.001f) {
			return false;
		}
	}
	return true;
}


