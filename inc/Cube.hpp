#ifndef CUBE_HPP
# define CUBE_HPP

# include <glfw3.h>
# include "Vec3.hpp"
# include "Mat4.hpp"
# include "Board.hpp"

class Cube
{

public:

    int				current_swap[2];

	struct Model
	{
		GLuint			v_mem_size;
		GLuint			v_num_elem;
		Vec3<GLfloat>	*v_array;
		GLuint			f_mem_size;
		GLuint			f_num_elem;
		GLubyte			*f_array;
		GLuint			n_mem_size;
		GLuint			n_num_elem;
		Vec3<GLfloat>	*n_array;
	};

	struct CubeMat
	{
		Mat4<GLfloat>	matrix;
		Mat4<GLfloat>	init_transf;
		Mat4<GLfloat>	anim_transf;
		Mat4<GLfloat>	move;
	};

	float			move_speed;
	float			move_counter;
	float			space_btw_cube;

	int				move_cycle;

	CubeMat			*matrix_array;
	Model			model;
	Board			*board;

	Cube(void);
	~Cube(void);

	void			setDefaultValues();
	void			init(Board *board);

	void			increaseMoveSpeed(void);
	void			decreaseMoveSpeed(void);

	void			initMatrixArray(void);
	void			findAndApplyMoveDirection(void);
	int				createMoveAnimationFromSwapList(void);
	float			applyTransformation(void);
	int				hyperSpeed(int power = 42);
	void			swapInitTransformation(int ia, int ib);

	void			reset(void);

	void			generateModel(void);
};

#endif