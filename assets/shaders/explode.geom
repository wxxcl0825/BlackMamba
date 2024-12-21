#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
  vec2 uv;
  vec3 normal;
  vec3 worldPosition;
} gs_in[];

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

uniform float time;

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
  vec3 direction = normal * time;
  return position + vec4(direction, 0.0);
}

void main() {
  vec3 normalN = getNormal();

  gl_Position = explode(gl_in[0].gl_Position, normalN);
  uv = gs_in[0].uv;
  normal = gs_in[0].normal;
  worldPosition = gs_in[0].worldPosition;
  EmitVertex();
  gl_Position = explode(gl_in[1].gl_Position, normalN);
  uv = gs_in[1].uv;
  normal = gs_in[1].normal;
  worldPosition = gs_in[1].worldPosition;
  EmitVertex();
  gl_Position = explode(gl_in[2].gl_Position, normalN);
  uv = gs_in[2].uv;
  normal = gs_in[2].normal;
  worldPosition = gs_in[2].worldPosition;
  EmitVertex();
  EndPrimitive();
}