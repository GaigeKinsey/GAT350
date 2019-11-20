#version 430

#define POINT 0
#define DIRECTION 1
#define SPOTLIGHT 2
	
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
	int type;
	vec4 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float cutoff;
	float exponent;
};

uniform light_s light;

layout (binding = 0) uniform sampler2D texture_sample;

void phong(light_s in_light, vec3 position, vec3 normal, out vec3 ambient, out vec3 diffuse, out vec3 specular)
{
	vec3 position_to_light;
	if (in_light.type == POINT || in_light.type == SPOTLIGHT) position_to_light = normalize(vec3(in_light.position) - position);
	else if (in_light.type == DIRECTION) position_to_light = normalize(-in_light.direction);

	//ambient
	ambient = material.ambient * in_light.ambient;

	diffuse = vec3(0.0);
	specular = vec3(0.0);

	float spot_intensity = 1.0;

	if (in_light.type == SPOTLIGHT)
	{
		float cos_angle = dot(in_light.direction, -position_to_light);
		float angle = acos(cos_angle);

		if (angle > in_light.cutoff) return;

		spot_intensity = pow(cos_angle, in_light.exponent);
	}

	//diffuse
	float lDotN = max(dot(position_to_light, normal), 0.0);
	diffuse = ((in_light.diffuse * material.diffuse) * lDotN) * spot_intensity;

	//specular
	specular = vec3(0.0);
	if (lDotN > 0.0)
	{
		vec3 position_to_view = normalize(-position.xyz);
		vec3 light_reflect = reflect(-position_to_light, normal);
		float specular_intensity = max(dot(position_to_view, light_reflect), 0.0);
		specular_intensity = pow(specular_intensity, material.shininess);
		specular = (in_light.specular * material.specular * specular_intensity) * spot_intensity;
	}
}

void main()
{
	//if (mod(gl_FragCoord.y, 4.0) > 2.0) discard;
	//if (mod(gl_FragCoord.x, 4.0) > 2.0) discard;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	phong(light, fposition, fnormal, ambient, diffuse, specular);

	vec4 phong_color = vec4(ambient + diffuse, 1.0f) * texture(texture_sample, ftexcoord) + vec4(specular, 1.0f);
	phong_color = vec4(1.0) - phong_color;
	//phong_color = vec4(fnormal, 1.0);
	//phong_color = vec4(gl_FragCoord.z);

	color = phong_color;
}
