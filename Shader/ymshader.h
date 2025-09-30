#pragma once
#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>
#include <cmath>
#include <set>
#include <iomanip> // 출력 정렬용

#define pi 3.14159265358979323846

enum SHAPE { ZUM, LINE, TRIANGLE, RECTANGLE, PENTAGON };

struct VERTEX {
	float x, y, z;    // 위치
	float r, g, b;    // 색상
	int id;          // 정점 번호
};

class OBJECT {
public:
	int start_idx; // Vertexs에서 시작 인덱스
	int tri_cnt; // List에서 인덱스 개수
	int shape;

	OBJECT(int sid, int tri, SHAPE ishape) {
		start_idx = sid;
		tri_cnt = tri;
		shape = ishape;
	}
};


extern std::vector<VERTEX> Vertexs;
extern std::vector<unsigned int> List;
extern std::vector<OBJECT> Objects;
extern int v_count;
static float red, green, blue;

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
void CreateTriangle(GLfloat x, GLfloat y, float size) {
	RandomColor();
	float r = size / sqrt(3);
	float angle0 = pi / 2;
	float angle1 = angle0 + 2 * pi / 3;
	float angle2 = angle0 + 4 * pi / 3;
	Vertexs.push_back({ x + r * cos(angle0), y + r * sin(angle0), 0.0f, red, green, blue, v_count});
	List.push_back(v_count++);
	Vertexs.push_back({ x + r * cos(angle1), y + r * sin(angle1), 0.0f, red, green, blue, v_count});
	List.push_back(v_count++);									  
	Vertexs.push_back({ x + r * cos(angle2), y + r * sin(angle2), 0.0f, red, green, blue, v_count});
	List.push_back(v_count++);
}
void CreateRectangle(GLfloat x, GLfloat y, float size) {
	RandomColor();
	float half = size / 2;
	Vertexs.push_back({ x - half, y - half, 0.0f, red, green, blue, v_count++ });
	Vertexs.push_back({ x - half, y + half, 0.0f, red, green, blue, v_count++ });
	Vertexs.push_back({ x + half, y + half, 0.0f, red, green, blue, v_count++ });
	Vertexs.push_back({ x + half, y - half, 0.0f, red, green, blue, v_count++ });
	
	// 두 삼각형으로 구성
	List.push_back(v_count - 4); //0
	List.push_back(v_count - 3); //1
	List.push_back(v_count - 1); //3

	List.push_back(v_count-2); //2
	List.push_back(v_count - 1); //3
	List.push_back(v_count - 3); //1
}
void CreatePentagon(GLfloat x, GLfloat y, float size) {
	RandomColor();
	float r = size;
	float angle0 = -pi / 2; // 위쪽부터 시작

	// 중심점 추가
	Vertexs.push_back({ x, y, 0.0f, red, green, blue, v_count });
	int centerIdx = v_count++;

	// 꼭짓점 5개 추가
	std::vector<int> idxs;
	for (int i = 0; i < 5; ++i) {
		float angle = angle0 + i * 2 * pi / 5;
		float vx = x + r * cos(angle);
		float vy = y + r * sin(angle);
		Vertexs.push_back({ vx, vy, 0.0f, red, green, blue, v_count });
		idxs.push_back(v_count++);
	}

	// 삼각형 5개 인덱스 추가
	for (int i = 0; i < 5; ++i) {
		List.push_back(centerIdx);
		List.push_back(idxs[i]);
		List.push_back(idxs[(i + 1) % 5]);
	}
}

void AddShape(float x, float y, SHAPE type) {
	int list_idx_start = List.size();
	if (type == ZUM) {
		CreateRectangle(x, y, 0.01f);
	}
	else if (type == LINE) {
		RandomColor();
		float half = 0.005f;
		Vertexs.push_back({ x - (10 * half), y - half, 0.0f, red, green, blue, v_count++ });
		Vertexs.push_back({ x - (10 * half), y + half, 0.0f, red, green, blue, v_count++ });
		Vertexs.push_back({ x + (10 * half), y + half, 0.0f, red, green, blue, v_count++ });
		Vertexs.push_back({ x + (10 * half), y - half, 0.0f, red, green, blue, v_count++ });
		List.push_back(v_count - 4); List.push_back(v_count - 3); List.push_back(v_count - 1);
		List.push_back(v_count - 2); List.push_back(v_count - 1); List.push_back(v_count - 3);
	}
	else if (type == TRIANGLE) {
		CreateTriangle(x, y, 0.2f);
	}
	else if (type == RECTANGLE) {
		CreateRectangle(x, y, 0.2f);
	}
	else if (type == PENTAGON) {
		CreatePentagon(x, y, 0.1f);
	}
	Objects.push_back(OBJECT(list_idx_start, (int)List.size() - list_idx_start, type));
}

void MoveObject(int objIdx, float dx, float dy) {
	int start = Objects[objIdx].start_idx;
	int cnt = Objects[objIdx].tri_cnt;
	std::set<int> moved; // 중복 방지
	for (int i = 0; i < cnt; ++i) {
		int vi = List[start + i];
		if (moved.find(vi) == moved.end()) {
			Vertexs[vi].x += dx;
			Vertexs[vi].y += dy;
			moved.insert(vi);
		}
	}
}

inline void PrintObjectInfo(int objIdx) {
	if (objIdx < 0 || objIdx >= Objects.size()) {
		std::cout << "Invalid OBJECT index: " << objIdx << std::endl;
		return;
	}
	const OBJECT& obj = Objects[objIdx];
	std::cout << "=== OBJECT " << objIdx << " ===" << std::endl;
	std::cout << "  start_idx: " << obj.start_idx
		<< ", tri_cnt: " << obj.tri_cnt
		<< ", shape: " << obj.shape << std::endl;

	std::cout << "  Indices: ";
	for (int i = 0; i < obj.tri_cnt; ++i) {
		std::cout << List[obj.start_idx + i] << " ";
	}
	std::cout << std::endl;

	std::cout << "  Vertices:" << std::endl;
	std::set<int> printed;
	for (int i = 0; i < obj.tri_cnt; ++i) {
		int vi = List[obj.start_idx + i];
		if (printed.find(vi) == printed.end()) {
			const VERTEX& v = Vertexs[vi];
			std::cout << std::fixed << std::setprecision(1)
				<< "    [" << vi << "] "
				<< "x: " << v.x << ", y: " << v.y << ", z: " << v.z
				<< " | color: (" << v.r << ", " << v.g << ", " << v.b << ")"
				<< " | id: " << v.id << std::endl;
			printed.insert(vi);
		}
	}
	std::cout << "======================" << std::endl;
}