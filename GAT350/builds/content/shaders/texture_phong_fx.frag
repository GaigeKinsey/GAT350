#version 430
	
in vec3 fposition;
in vec3 fnormal;
in vec2 ftexcoord;

out vec4 color;
	
struct material_s
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess; 
};

uniform material_s material;

struct light_s
{
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform light_s light;

uniform vec3 discard_color;
uniform float dissolve;

layout (binding = 0) uniform sampler2D texture_sample;
layout (binding = 1) uniform sampler2D noise_sample;

void main()
{
	vec4 texture_color = texture(texture_sample, ftexcoord);
	//if (discard_color == texture_color.rgb) discard;
	if (texture(noise_sample, ftexcoord).r <= dissolve) discard;
	

	vec3 position_to_light = normalize(vec3(light.position) - fposition);

	//ambient
	vec3 ambient = light.ambient * material.ambient;

	//diffuse
	float lDotN = max(dot(position_to_light, fnormal), 0.0);
	vec3 diffuse = light.diffuse * material.diffuse * lDotN;

	//specular
	vec3 specular = vec3(0.0);
	if (lDotN > 0.0)
	{
		vec3 position_to_view = normalize(-fposition.xyz);
		vec3 light_reflect = reflect(-position_to_light, fnormal);
		float intensity = max(dot(position_to_view, light_reflect), 0.0);
		intensity = pow(intensity, material.shininess);
		specular = light.specular * material.specular * intensity;
	}

	color = vec4(ambient + diffuse, 1.0f) * texture(texture_sample, ftexcoord) + vec4(specular, texture_color.a);
}
