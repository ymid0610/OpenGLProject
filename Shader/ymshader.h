#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>

#define pi 3.14159265358979323846

struct VERTEX {
	float x, y, z;    // 위치
	float r, g, b;    // 색상
	int id;          // 정점 번호
};

extern std::vector<VERTEX> Vertexs;
extern std::vector<unsigned int> List;
extern int v_count;

static float red, green, blue;

GLfloat ConvertMouseToOGL_x(int x, int windowWidth) {
	return (2.0f * x) / windowWidth - 1.0f;
}
GLfloat ConvertMouseToOGL_y(int y, int windowHeight) {
	return 1.0f - (2.0f * y) / windowHeight;
}

void RandomColor() {
	red = (float)(rand() % 100) / 100.0f;
	green = (float)(rand() % 100) / 100.0f;
	blue = (float)(rand() % 100) / 100.0f;
}

void CreateTriangle(GLfloat x, GLfloat y, float size) {
	RandomColor();
	float r = size / sqrt(3);
	float angle0 = pi / 2;
	float angle1 = angle0 + 2 * pi / 3;
	float angle2 = angle0 + 4 * pi / 3;
	Vertexs.push_back({ x + r * cos(angle0), y + r * sin(angle0), 0.0f, 1.0f, 0.0f, 0.0f, v_count});
	List.push_back(v_count++);
	Vertexs.push_back({ x + r * cos(angle1), y + r * sin(angle1), 0.0f, 0.0f, 1.0f, 0.0f, v_count});
	List.push_back(v_count++);
	Vertexs.push_back({ x + r * cos(angle2), y + r * sin(angle2), 0.0f, 0.0f, 0.0f, 1.0f, v_count});
	List.push_back(v_count++);
}