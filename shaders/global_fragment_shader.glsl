#version 410

#define MAX_LIGHTS		9

struct Light
{
	vec3		position;
	vec3		color;
	float		shine;
	float		attenuation;
	float		ambient;
};

uniform sampler2D	tex;
uniform mat4		view_matrix, model_matrix;
uniform vec3		camera_pos;
uniform int			reverse_normal;

uniform	Light		light[MAX_LIGHTS];

in vec2				vS_tex_coord;
in vec3				vS_position;
in vec3				vS_normal;
in vec3				vS_position_trans;

out vec4			out_fragment;


void main()
{
	vec4    pixel_color = texture(tex, vS_tex_coord);
	vec3    view_forward = vec3(view_matrix[3]);

	// Adjust normal in world coordinates:
	vec3    normal = normalize(vec3(model_matrix * vec4(vS_normal, 1)));

	// Reverse normal inside cube:
	if (reverse_normal == 1)
		normal = -normal;

	// Get fragment position in world coordinates:
	vec3    frag_pos_world = vec3(model_matrix * vec4(vS_position, 1));

	vec3    surface_to_camera = normalize(camera_pos - frag_pos_world);

	vec3    final_color = vec3(0.0);
	vec3    no_tex_color = vec3(0.0);

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		vec3	surface_to_light = normalize(light[i].position - frag_pos_world);
		float	diffuse_coef = max(0.0, dot(normal, surface_to_light));
		vec3	diffuse = diffuse_coef * light[i].color * pixel_color.rgb;
		float	specular_coef = 0.0;
		if (diffuse_coef > 0.0)
			    specular_coef = pow(max(0.0, dot(normal, normalize(surface_to_camera + surface_to_light))), light[i].shine);
		vec3    specular = specular_coef * light[i].color;
		float	attenuation_coef = 1.0 / (1.0 + light[i].attenuation * pow(distance(light[i].position, frag_pos_world), 2));
		final_color += (light[i].ambient * pixel_color.rgb) + attenuation_coef * (diffuse + specular);
		no_tex_color += light[i].ambient * vec3(0.2) + attenuation_coef * (diffuse_coef * light[i].color + specular);
	}
	if (vS_position.z >= 0.499 || reverse_normal == 1)
		out_fragment = vec4(final_color.rgb, pixel_color.a);
	else
		out_fragment = vec4(no_tex_color.rgb, 1.0);
}