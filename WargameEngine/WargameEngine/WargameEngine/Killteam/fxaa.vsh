#version 130

out vec2 v_texcoord0;

void main() {
  v_texcoord0 = gl_MultiTexCoord0.st;
  gl_Position = ftransform();
}
