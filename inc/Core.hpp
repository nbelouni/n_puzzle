#ifndef	CORE_HPP
# define CORE_HPP

# ifndef M_PI
#  define M_PI		3.1415926535f
# endif

# if defined(__APPLE_CC__)
#  ifndef GLFW_INCLUDE_GLCOREARB
#   define GLFW_INCLUDE_GLCOREARB
#  endif
#  ifndef GLFW_INCLUDE_GLEXT
#   define GLFW_INCLUDE_GLEXT
#  endif
# else
#  define GL_GLEXT_PROTOTYPES
# endif

# define VBOS_NUM			3
# define SHADER_NUM			1

# include <iostream>
# include <mutex>
# include <thread>
# include "Shaders.hpp"
# include "Mat4.hpp"
# include "Camera.hpp"
# include "Texture.hpp"
# include "Cube.hpp"
# include "Board.hpp"
# include "Algo.hpp"
# include "Light.hpp"
# include "Animator.hpp"
# include "../compiled_GLFW/glfw3.h"

enum vbos
{
	MODEL_VERTICES,
	MODEL_FACES,
	MODEL_NORMALS,
};

enum shaders
{
	GLOBAL_SHADER
};

enum invisible_cube_toggle
{
	SHOW,
	HIDE
};

enum shuffle_type
{
    NONE,
    VALID,
    RANDOM
};

enum game_stage
{
    NOTHING,
    PLAY,
    STOP
};

class Core
{

public:

	GLFWwindow		*window;
	Camera			camera;
	Texture			texture;
	GLuint			vao;
	GLuint			vbos[VBOS_NUM];

	Board			board;
	Algo			algo;
	Cube			cube;
	Light			light;
	Animator		animator;

	float			all_cubes_width;
    double          time_to_solve;

	int				status;

	int				pause;
    int             heuristic;
	int				game_mode;
    int             hyper_speed;
    int             ultra_speed;
    int             shuffle_type;
    int             hyper_stage;
	int				game_stage;
	int				move_stage;
	int				reset_stage;
	int				solv_stage;
	int				unsolv_stage;
	int				light_move;
	int				light_rot;
	int				light_blink;
	int				light_bounce;
	int				invisible_cube_toggle;
	int				texture_selection;

    float           camera_ideal_pos;
    int             size_select;
    int             size_select_active;

    std::thread     *th = NULL;

    Shaders			shaders;
	GLuint			program[SHADER_NUM];

	GLuint			res_x;
	GLuint			res_y;

	Mat4<GLfloat>	proj_matrix;

	GLuint			position_loc;
	GLuint			normal_loc;
	GLuint			proj_loc;
	GLuint			view_loc;
	GLuint			model_loc;
	GLuint			texture_loc;
	GLuint			camera_pos_loc;
	GLuint			board_size_x_loc;
	GLuint			cube_coord_x_loc;
	GLuint			cube_coord_y_loc;

	Core(void);
	~Core(void);

	void		init(void);
    void        sizeSelector(int digit);
	void		changeSize(int side_size);

	void		setupMatrices(void);
	void		setProjMatrix(GLfloat fov, GLfloat near_cp, GLfloat far_cp);

	void		genShaders(void);

	void		createImage(void);
	void		buildScene(void);

	void		createAndLoadTextures(void);

    void        gameStageManager(void);
    void        hyperStageManager(void);
    void		resetAnimation(void);
    void		lightAnimation(void);
    void		moveAnimation(void);
    void		solvAnimation(void);
    void		unsolvAnimation(void);
	void		updateEnvironment(int current_tick);

	void		resetLight(void);

	void		resolve(void);
	void		drawImage(void);

	void		checkGlError(std::string file, int line);
};

#endif
