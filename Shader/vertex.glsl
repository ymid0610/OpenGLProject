#version 330 core
layout(location = 0) in vec3 vPos;    // 위치
layout(location = 1) in vec3 vColor;  // 색상

uniform mat4 modelTransform;

out vec3 outColor; // 프래그먼트 세이더로 넘길 색상

void main()
{
    gl_Position = modelTransform * vec4(vPos, 1.0);
    outColor = vColor; // 반드시 할당!
}