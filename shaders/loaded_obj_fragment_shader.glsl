#version 460

in vec3 frag_pos;
in vec3 frag_color;
in vec2 frag_texCoord;
in vec3 frag_normal;

in float frag_ambient;

uniform sampler2D tex;
uniform float ambient_strength;
uniform vec3 light_pos;

out vec4 out_color;

void main(void) {
	vec3 normal = normalize(frag_normal);
	vec3 light_dir = normalize(light_pos - frag_pos);

	vec3 ambient = ambient_strength * frag_color;

	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = diff * frag_color;

	vec3 result = ambient + diffuse;

	out_color = texture(tex, frag_texCoord) * vec4(frag_color, 1.0) * vec4(result, 1.0);

	if(out_color.a < 0.1)
		discard;
}