#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 uLightProjection;
uniform mat4 uWorldToLight[6];

out vec4 fragPos;

void main() {
  for (int face = 0; face < 6; ++face) {
    gl_Layer = face;
    for (int i = 0; i < 3; ++i) {
      fragPos = gl_in[i].gl_Position;
      gl_Position = uLightProjection * uWorldToLight[face] * fragPos;
      EmitVertex();
    }
    EndPrimitive();
  }
}
