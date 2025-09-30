#pragma once
#include <iostream>

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

int CheckQuadrant(float x, float y) {
    if (x >= 0.0 && y >= 0.0) return 1; // 1사분면
    else if (x < 0.0 && y >= 0.0) return 2; // 2사분면
    else if (x < 0.0 && y < 0.0) return 3; // 3사분면
    else return 4; // 4사분면  
}