#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

void main() {
    gl_Position = vec4(aPosition, 1.0);
}