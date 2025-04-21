#version 410 core

#define MAX_LIGHT_NUM 5

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

out vec4 FragColor;

struct DirectionalLight {
  vec3 color;
  vec3 direction;
  float specularIntensity;
};

struct PointLight {
  vec3 position;
  vec3 color;
  float specularIntensity;
  float k2;
  float k1;
  float kc;
};

struct SpotLight {
  vec3 position;
  vec3 color;
  vec3 direction;
  float specularIntensity;
  float inner;
  float outer;
};

struct AmbientLight {
  vec3 color;
};

uniform sampler2D diffuse;
uniform float shiness;

uniform vec3 cameraPosition;

uniform int numDirectionalLight;
uniform DirectionalLight directionalLight[MAX_LIGHT_NUM];

uniform int numPointLight;
uniform PointLight pointLight[MAX_LIGHT_NUM];

uniform int numSpotLight;
uniform SpotLight spotLight[MAX_LIGHT_NUM];

uniform AmbientLight ambientLight;

uniform sampler2D alphaMap;
uniform float useAlphaMap;

vec3 calDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal) {
  float diffuse = clamp(dot(-lightDir, normal), 0.0, 1.0);
  return lightColor * diffuse * objectColor;
}

vec3 calSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir,
                 float intensity) {
  vec3 lightReflect = normalize(reflect(lightDir, normal));
  float specular = clamp(dot(lightReflect, -viewDir), 0.0, 1.0);
  specular = pow(specular, shiness);
  float back = step(0.0, dot(-lightDir, normal));
  return lightColor * specular * intensity * back;
}

vec3 calDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(light.direction);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return diffuseColor + specularColor;
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(worldPosition - light.position);
  float dist = length(worldPosition - light.position);
  float attenuation =
      1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return (diffuseColor + specularColor) * attenuation;
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(worldPosition - light.position);
  vec3 targetDir = normalize(light.direction);
  float cGamma = dot(lightDir, targetDir);
  float intensity =
      clamp((cGamma - light.outer) / (light.inner - light.outer), 0.0, 1.0);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return (diffuseColor + specularColor) * intensity;
}

vec3 calAmbientLight(AmbientLight light) {
  return light.color * texture(diffuse, uv).xyz;
}

void main() {
  vec3 normalN = normalize(normal);
  vec3 viewDir = normalize(worldPosition - cameraPosition);

  vec3 result = vec3(0.0);

  for (int i = 0; i < numDirectionalLight; i++)
    result += calDirectionalLight(directionalLight[i], normalN, viewDir);
  for (int i = 0; i < numPointLight; i++)
    result += calPointLight(pointLight[i], normalN, viewDir);
  for (int i = 0; i < numSpotLight; i++)
    result += calSpotLight(spotLight[i], normalN, viewDir);
  result += calAmbientLight(ambientLight);

  float alphaValue = mix(1.0, texture(alphaMap, uv).r, useAlphaMap);
  FragColor = vec4(result, alphaValue);
}