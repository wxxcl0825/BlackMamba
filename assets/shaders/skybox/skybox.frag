#version 410 core
out vec4 FragColor;

in vec3 uvw;

uniform samplerCube sampler;

void main() { FragColor = texture(sampler, uvw); }