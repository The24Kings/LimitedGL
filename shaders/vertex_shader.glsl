#version 330 core

in vec3 i_position;

void main()
{
  gl_Position = vec4(i_position, 1);
}