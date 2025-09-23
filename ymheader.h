#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

GLfloat ConvertMouseToOpenGLX(int x, int windowWidth) {
	return (2.0f * x) / windowWidth - 1.0f;
}

GLfloat ConvertMouseToOpenGLY(int y, int windowHeight) {
	return 1.0f - (2.0f * y) / windowHeight;
}

GLfloat RandomValue() {
	return -1.0f + ((rand() % 200) * 0.01f);
}

enum MOVE { STOP, CROSS, ZIGZAG, SIZECHANGE, COLORCHANGE, FOLLOW, RESET };

class YMRECT {
public:
	GLfloat x, y, size;
	GLfloat red, green, blue;
	GLfloat x1, y1, x2, y2;
	int move_type = STOP;

	GLfloat CreatedX, CreatedY;

	YMRECT() {
		random_color();
	}
	YMRECT(float ix, float iy, float isize) {
		x = ix, y = iy; size = isize;
		x1 = x - size; y1 = y - size; x2 = x + size; y2 = y + size;
		random_color();
	}
	void random_color() {
		red = 0.0f + ((rand() % 100) * 0.01f);
		green = 0.0f + ((rand() % 100) * 0.01f);
		blue = 0.0f + ((rand() % 100) * 0.01f);
	}
	bool is_mouse_inside(GLfloat px, GLfloat py) {
		if (px >= x1 && px <= x2 && py >= y1 && py <= y2) return true;
		else return false;
	}
	bool is_rect_inside(const YMRECT& rect) {
		// 두 사각형이 전혀 겹치지 않는 경우를 먼저 체크
		if (x2 < rect.x1 || x1 > rect.x2 || y2 < rect.y1 || y1 > rect.y2) {
			return false;
		}
		// 그 외에는 겹침(충돌)
		return true;
	}
	void remake_x1y1x2y2() {
		x1 = x - size; y1 = y - size; x2 = x + size; y2 = y + size;
	}
	void remake_xy_size() {
		x = (x1 + x2) / 2.0f;
		y = (y1 + y2) / 2.0f;
		size = (x2 - x1) / 2.0f;
		if (size > (y2 - y1) / 2.0f) size = (y2 - y1) / 2.0f;
	}
	void size_up(float max) {
		if (size >= max) return;
		size += 0.05f;
		remake_x1y1x2y2();
	}
	void size_down(float min) {
		if (size <= min) return;
		size -= 0.01f;
		remake_x1y1x2y2();
	}
	void union_rect(YMRECT rect) {
		x1 = (x1 < rect.x1) ? x1 : rect.x1;
		y1 = (y1 < rect.y1) ? y1 : rect.y1;
		x2 = (x2 > rect.x2) ? x2 : rect.x2;
		y2 = (y2 > rect.y2) ? y2 : rect.y2;
		remake_xy_size();
		random_color();
	}
	YMRECT devide_rect() {
		int random = rand() % 10;
		for (int i = 0; i < random + 1; i++) {
			size_down(0.05f);
		}
		remake_x1y1x2y2();

		YMRECT temp;
		temp.x = x + 2 * size; temp.y = y; temp.size = size;
		temp.remake_x1y1x2y2();
		return temp;
	}
	void move_by_mouse(GLfloat dx, GLfloat dy) {
		x = dx; y = dy;
		remake_x1y1x2y2();
	}
};