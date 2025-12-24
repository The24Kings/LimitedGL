#version 460 core

in vec3 frag_pos;
in vec3 frag_color;
in vec2 frag_texCoord;
in vec3 frag_normal;

in float frag_ambient;

uniform sampler2D tex;
uniform float ambient_strength;
uniform float specular_strength;
uniform vec3 light_pos;
uniform vec3 view_pos;

out vec4 out_color;

void main(void) {
	// Ambient
	vec3 ambient = ambient_strength * frag_color;
	
	// Diffuse
	vec3 normal = normalize(frag_normal);
	vec3 light_dir = normalize(light_pos - frag_pos);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = diff * frag_color;

	// Specular
	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * frag_color;

	vec3 result = ambient + diffuse + specular;

	out_color = vec4(result, 1.0) * texture(tex, frag_texCoord) * vec4(frag_color, 1.0);

	if(out_color.a < 0.1)
		discard;
}