#version 410

uniform mat4	proj_matrix, view_matrix, model_matrix;
uniform	float	board_size_x, cube_coord_x, cube_coord_y;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec2	vS_tex_coord;
out vec3	vS_position;
out vec3	vS_normal;

void		main()
{
	float	cuts = 1.0 / board_size_x;

	vec3 tex_coord = (position + 0.5);
	vS_tex_coord = vec2(tex_coord.x * cuts + (cube_coord_x * cuts), -tex_coord.y * cuts + ((cube_coord_y + 1) * cuts));

	vS_position = position;
	vS_normal = normal;
	gl_Position = proj_matrix
				  * view_matrix
				  * model_matrix
				  * vec4(position, 1.0);
}
