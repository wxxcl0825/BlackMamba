#version 410 core

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

out vec4 FragColor;

struct AmbientLight {
  vec3 color;
};

uniform sampler2D diffuse;
uniform sampler2D opacity;

uniform float transparency;

uniform AmbientLight ambientLight;

vec3 calAmbientLight(AmbientLight light) {
  return light.color * texture(diffuse, uv).xyz;
}

void main() {
  vec3 result = calAmbientLight(ambientLight);
  float alpha = texture(opacity, uv).r;

  FragColor = vec4(result, transparency * alpha);
}