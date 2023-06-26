#version 330

in vec4 fragPos;

uniform vec3 uLightPosition;
uniform float uFarPlane;

void main() {
  float lightDistance = length(fragPos.xyz - uLightPosition);
  lightDistance = lightDistance / uFarPlane;
  gl_FragDepth = lightDistance;
}
