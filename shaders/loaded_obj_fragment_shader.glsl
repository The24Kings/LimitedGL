#version 460

in vec2 frag_texCoord;
out vec4 outcolor;
uniform sampler2D tex;

void main(void) {
  outcolor = texture(tex, frag_texCoord);
  if(outcolor.a < 0.1)
		discard;
}