#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

out VS_OUT {
  vec2 uv;
  vec3 normal;
  vec3 worldPosition;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
  vec4 transformPosition = model * vec4(aPosition, 1.0);

  gl_Position = projection * view * transformPosition;
  vs_out.uv = uv;
  vs_out.normal = normal;
  vs_out.worldPosition = worldPosition;
  uv = aUv;
  normal = normalMatrix * aNormal;
  worldPosition = transformPosition.xyz;
}