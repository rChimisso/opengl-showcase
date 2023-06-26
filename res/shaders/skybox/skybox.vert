#version 330

layout(location = 0) in vec3 uPosition;

out vec3 vPosition;

uniform mat4 uProjection;
uniform mat4 uView;

void main() {
  vPosition = uPosition;
  gl_Position = (uProjection * uView * vec4(uPosition, 1.0)).xyww;
}
