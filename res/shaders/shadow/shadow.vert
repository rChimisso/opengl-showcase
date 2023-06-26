#version 330

layout(location = 0) in vec3 lPosition;

uniform mat4 uModelToWorld;

void main() {
  gl_Position = uModelToWorld * vec4(lPosition, 1.0);
}
