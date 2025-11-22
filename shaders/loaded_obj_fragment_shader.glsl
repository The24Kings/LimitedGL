#version 460

in vec3 frag_color;
in vec2 frag_texCoord;

uniform sampler2D tex;

out vec4 out_color;

void main(void) {
  out_color = texture(tex, frag_texCoord) * vec4(frag_color, 1.0);

  if(out_color.a < 0.1)
		discard;
}