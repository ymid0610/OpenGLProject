#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>

struct FLOATXY {
	GLfloat x, y;
};

GLfloat ConvertMouseToOpenGLX(int x, int windowWidth) {
	return (2.0f * x) / windowWidth - 1.0f;
}

GLfloat ConvertMouseToOpenGLY(int y, int windowHeight) {
	return 1.0f - (2.0f * y) / windowHeight;
}

GLfloat RandomValue() {
	return -1.0f + ((rand() % 200) * 0.01f);
}

GLfloat RandomHalfValue() {
	return 0.0f + ((rand() % 100) * 0.01f);
}

GLfloat RandomSizeValue() {
	return 0.05f + ((rand() % 150) * 0.001f);
}

enum MOVE { STOP, CROSS, ZIGZAG, FOLLOW, RESET};
enum CHANGE {NOTHING, SIZECHANGE, COLORCHANGE };
enum DISAPPEAR { NONE, LEFT, RIGHT, UP, DOWN, LEFTUP, RIGHTUP, LEFTDOWN, RIGHTDOWN };

class YMRECT {
public:
	GLfloat x, y, size;
	GLfloat red, green, blue;
	GLfloat x1, y1, x2, y2;
	int move_type = STOP;
	int change_type = NOTHING;
	int disappear_type = NONE;
	bool eraser = false;
	float dix = 1, diy = 1, dis = 1;
	GLfloat CreatedX, CreatedY;

	YMRECT() {
		random_color();
	}
	YMRECT(float ix, float iy, float isize) {
		CreatedX = ix; CreatedY = iy;
		x = ix, y = iy; size = isize;
		x1 = x - size; y1 = y - size; x2 = x + size; y2 = y + size;
		random_color();
	}
	YMRECT(float ix1, float iy1, float ix2, float iy2, GLfloat ired, GLfloat igreen, GLfloat iblue, int itype) {
		x1 = ix1; y1 = iy1; x2 = ix2; y2 = iy2;
		remake_xy_size();
		red = ired; green = igreen; blue = iblue;
		disappear_type = itype;
	}
	void random_color() {
		red = 0.0f + ((rand() % 100) * 0.01f);
		green = 0.0f + ((rand() % 100) * 0.01f);
		blue = 0.0f + ((rand() % 100) * 0.01f);
	}
	void random_size() {
		std::cout << "random_size\n";
		if (size >= 0.2f) dis = -1;
		else if (size <= 0.05f) dis = 1;
		size += dis * 0.01f;
		remake_x1y1x2y2();
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
	void move_cross() {
		std::cout << "move_cross\n";
		if (x2 >= 1.0f || x1 <= -1.0f) {
			dix = -dix;
		}
		if (y2 >= 1.0f || y1 <= -1.0f) {
			diy = -diy;
		}
		x += dix * 0.02f;
		y += diy * 0.02f;
		remake_x1y1x2y2();
	}
	void move_zigzag() {
		std::cout << "move_zigzag\n";
		if (x2 >= 1.0f || x1 <= -1.0f) {
			dix = -dix;
		}
		if (y2 >= 1.0f || y1 <= -1.0f) {
			dix = -dix;
			diy = -diy;
		}
		x += dix * 0.02f;
		y += diy * 0.01f;
		remake_x1y1x2y2();
	}
	FLOATXY move_follow(GLfloat prior_x, GLfloat prior_y, float dix, float diy) {
		std::cout << "move_follow\n";
		x = prior_x; y = prior_y;
		remake_x1y1x2y2();
		FLOATXY temp;
		if (dix >= 1.0f && diy >= 1.0f) {
			temp = { x - size, y - size };
		}
		else if (dix <= -1.0f && diy >=1.0f) {
			temp = { x + size, y - size };
		}
		else if (dix >= 1.0f && diy <= -1.0f) {
			temp = { x - size, y + size };
		}
		else if (dix <= -1.0f && diy <= -1.0f) {
			temp = { x + size, y + size };
		}
		return temp;
	}
	void move_reset() {
		if (x != CreatedX && y != CreatedY) {
			// 두점사이의 거리공식을 사용하여 원래 위치로 돌아가기
			float angle = atan2(CreatedY - y, CreatedX - x);
			x += cos(angle) * 0.02f;
			y += sin(angle) * 0.02f;
			if (sqrt((CreatedX - x) * (CreatedX - x) + (CreatedY - y) * (CreatedY - y)) < 0.02f) {
				x = CreatedX;
				y = CreatedY;
			}
		}
		else {
			move_type = STOP;
		}
		remake_x1y1x2y2();
	}
	std::vector<YMRECT> random_animation() {
		std::vector<YMRECT> new_rects;
		int random = rand() % 4;
		if (random == 0) {
			new_rects.push_back(YMRECT(x, y, x2, y2, red, green, blue, DOWN));
			new_rects.push_back(YMRECT(x1, y1, x, y, red, green, blue, UP));
			new_rects.push_back(YMRECT(x, y1, x2, y, red, green, blue, RIGHT));
			new_rects.push_back(YMRECT(x1, y, x, y2, red, green, blue, LEFT));
			
		}
		else if (random == 1) {
			new_rects.push_back(YMRECT(x, y, x2, y2, red, green, blue, LEFTUP));
			new_rects.push_back(YMRECT(x1, y1, x, y, red, green, blue, LEFTUP));
			new_rects.push_back(YMRECT(x, y1, x2, y, red, green, blue, LEFTUP));
			new_rects.push_back(YMRECT(x1, y, x, y2, red, green, blue, LEFTUP));
		}
		else if (random == 2) {
			new_rects.push_back(YMRECT(x, y, x2, y2, red, green, blue, LEFT));
			new_rects.push_back(YMRECT(x1, y1, x, y, red, green, blue, LEFT));
			new_rects.push_back(YMRECT(x, y1, x2, y, red, green, blue, LEFT));
			new_rects.push_back(YMRECT(x1, y, x, y2, red, green, blue, LEFT));
		}
		else if (random == 3) {
			new_rects.push_back(YMRECT(x, y, x2, y2, red, green, blue, DOWN));
			new_rects.push_back(YMRECT(x, y, x2, y2, red, green, blue, LEFTDOWN));
			new_rects.push_back(YMRECT(x1, y1, x, y, red, green, blue, UP));
			new_rects.push_back(YMRECT(x1, y1, x, y, red, green, blue, RIGHTUP));
			new_rects.push_back(YMRECT(x, y1, x2, y, red, green, blue, RIGHT));
			new_rects.push_back(YMRECT(x, y1, x2, y, red, green, blue, RIGHTDOWN));
			new_rects.push_back(YMRECT(x1, y, x, y2, red, green, blue, LEFT));
			new_rects.push_back(YMRECT(x1, y, x, y2, red, green, blue, LEFTUP));
		}

		return new_rects;
	}
	void disappear() {
		size -= 0.001f;
		remake_x1y1x2y2();
		if (disappear_type == UP) {
			disappear_move_up();
		}
		else if (disappear_type == DOWN) {
			disappear_move_down();
		}
		else if (disappear_type == LEFT) {
			disappear_move_left();
		}
		else if (disappear_type == RIGHT) {
			disappear_move_right();
		}
		else if (disappear_type == LEFTUP) {
			disappear_move_left();
			disappear_move_up();
		}
		else if (disappear_type == RIGHTUP) {
			disappear_move_right();
			disappear_move_up();
		}
		else if (disappear_type == LEFTDOWN) {
			disappear_move_left();
			disappear_move_down();
		}
		else if (disappear_type == RIGHTDOWN) {
			disappear_move_right();
			disappear_move_down();
		}
	}
	void disappear_move_up() {
		y -= 0.01f;
		remake_x1y1x2y2();
	}
	void disappear_move_down() {
		y += 0.01f;
		remake_x1y1x2y2();
	}
	void disappear_move_left() {
		x -= 0.01f;
		remake_x1y1x2y2();
	}
	void disappear_move_right() {
		x += 0.01f;
		remake_x1y1x2y2();
	}
};