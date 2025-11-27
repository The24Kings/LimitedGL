#version 460 core

in vec3 in_vertex;
in vec3 in_color;
in vec2 in_texCoord;
in vec3 in_normal;

uniform mat4 model;
uniform mat4 vp;

out vec3 frag_pos;
out vec3 frag_color;
out vec2 frag_texCoord;
out vec3 frag_normal;

void main(void) {
	frag_pos = vec3(model * vec4(in_vertex, 1.0));

	frag_color = in_color;
	frag_texCoord = in_texCoord;
	frag_normal = mat3(transpose(inverse(model))) * in_normal; // scaled model fixes

	gl_Position = vp * vec4(frag_pos, 1.0); // mvp is reveresed because matrix mult
}