#version 330 core

out vec4 fragColor;

in vec3 vPosition;

uniform samplerCube uSkybox;

void main() {
  fragColor = texture(uSkybox, vPosition);
}
