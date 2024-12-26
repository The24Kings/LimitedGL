#version 460

in vec3 in_vertex;
in vec2 in_texCoord;
uniform mat4 vp;
out vec2 frag_texCoord;
out vec4 gl_Position;

void main(void) {	
	gl_Position = vp * vec4(in_vertex, 1.0);
	frag_texCoord = in_texCoord;
}