#version 410 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D diffuse;

void main() { FragColor = texture(diffuse, uv); }