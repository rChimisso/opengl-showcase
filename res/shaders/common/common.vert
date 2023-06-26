#version 330

layout (location = 0) in vec3 lPosition;
layout (location = 1) in vec2 lUv;
layout (location = 2) in vec3 lNormal;
layout (location = 3) in vec3 lNormalTangent;
layout (location = 4) in vec3 lNormalBitangent;

uniform mat4 uModelToWorld;
uniform mat4 uWorldToCamera;
uniform mat4 uCameraProjection;

out vec3 vPosition;
out vec2 vUv;
out vec3 vNormal;
out vec3 vNormalTangent;
out vec3 vNormalBitangent;

void main() {
  gl_Position = uCameraProjection * uWorldToCamera * uModelToWorld * vec4(lPosition, 1.0);
  mat4 modelToWorldIT = transpose(inverse(uModelToWorld));
  vPosition = vec3(uModelToWorld * vec4(lPosition, 1.0));
  vUv = lUv;
  vNormal = normalize(vec3(modelToWorldIT * vec4(lNormal, 0.0)));
  vNormalTangent = normalize(vec3(modelToWorldIT * vec4(lNormalTangent, 0.0)));
  vNormalBitangent = normalize(vec3(modelToWorldIT * vec4(lNormalBitangent, 0.0)));
}
