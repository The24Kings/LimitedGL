#version 460

in vec3 in_vertex;
in vec3 in_color;
in vec2 in_texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_color;
out vec2 frag_texCoord;

void main(void) {	
	gl_Position = projection * view * model * vec4(in_vertex, 1.0); // mvp is reveresed because matrix mult
	frag_color = in_color;
	frag_texCoord = in_texCoord;
}