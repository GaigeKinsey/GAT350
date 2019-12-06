#version 430
	
in vec3 freflection;
in vec3 frefraction;

out vec4 color;

uniform float mix_value;

layout (binding = 0) uniform samplerCube texture_sample;

void main()
{
	vec4 reflect_color = texture(texture_sample, freflection);
	vec4 refract_color = texture(texture_sample, frefraction);

	color = mix(reflect_color, refract_color, mix_value);
}
