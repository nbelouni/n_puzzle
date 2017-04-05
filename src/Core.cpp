#include <OpenGL/OpenGL.h>
#include "../inc/Core.hpp"
#include "Algo.hpp"
# include <thread>
# include <mutex>

Core::Core(void)
{
	return;
}

Core::~Core(void)
{
	glDeleteBuffers(VBOS_NUM, vbos);
	return;
}

void
Core::init(void)
{
    time_to_solve = 0.0f;
    all_cubes_width = 0.0f;
	status = -1;
	pause = 0;
    heuristic = MANHATTAN1;
	game_mode = 0;
    hyper_speed = 0;
    ultra_speed = 0;
	shuffle_type = NONE;
    hyper_stage = NOTHING;
	game_stage = NOTHING;
	reset_stage = OFF;
	solv_stage = OFF;
	unsolv_stage = OFF;
	move_stage = OFF;
	light_move = 0;
	light_rot = 0;
	light_blink = 0;
	light_bounce = 0;
	invisible_cube_toggle = HIDE;
	texture_selection = BIOSHOCK_TEXTURE;
    camera_ideal_pos = 0.0f;
    size_select = 0;
    size_select_active = 0;
	window = NULL;
	vao = 0;
	vbos[0] = 0;
	vbos[1] = 0;
	vbos[2] = 0;
	res_x = 2560;
	res_y = 1440;
	camera.init();
	board.initSolved(30);
	board.validShuffle();
	cube.init(&board);
}

void
Core::sizeSelector(int digit)
{
    if (size_select == 0 && size_select_active == 0)
    {
        size_select = digit;
        size_select_active = 1;
    }
    else if (size_select_active == 1)
    {
        size_select *= 10;
        size_select += digit;
        size_select_active = 2;
    }
    else if (size_select_active == 2)
    {
        size_select *= 10;
        size_select += digit;
        size_select_active = 3;
    }
}

void
Core::changeSize(int side_size)
{
    if (th == 0)
    {
        move_stage = OFF;
        reset_stage = OFF;
        solv_stage = OFF;
        unsolv_stage = OFF;
        hyper_speed = 0;
        hyper_stage = NOTHING;
        game_mode = 0;
        game_stage = NOTHING;
        board.changeSize(side_size);
        board.validShuffle();
        cube.~Cube();
        cube.init(&board);
        cube.generateModel();
        animator.setDefaultValues();
        buildScene();
        resetLight();
        invisible_cube_toggle = HIDE;
        reset_stage = OFF;
        move_stage = OFF;
    }
}

void
Core::setupMatrices(void)
{
	setProjMatrix(90, 0.1, 1000);
}

void
Core::setProjMatrix(GLfloat fov, GLfloat near_cp, GLfloat far_cp)
{
	GLfloat		scale;
	GLfloat		ratio;

	scale = (GLfloat) (1.0f / tan(fov * 0.5 * M_PI / 180));
	ratio = (1.0f * res_x) / (1.0f * res_y);
	proj_matrix.set(0, scale / ratio);
	proj_matrix.set(5, scale);
	proj_matrix.set(10, (far_cp + near_cp) / (near_cp - far_cp));
	proj_matrix.set(11, -1.0f);
	proj_matrix.set(14, (2.0f * far_cp * near_cp) / (near_cp - far_cp));
	proj_matrix.set(15, 0.0f);
}

void
Core::genShaders(void)
{
	std::cerr << "Shader compilation:";

	shaders.load("./shaders/global_vertex_shader.glsl", "./shaders/global_fragment_shader.glsl");
	shaders.compile();
	program[GLOBAL_SHADER] = glCreateProgram();
	shaders.linkProgram(&program[GLOBAL_SHADER]);
	position_loc = glGetAttribLocation(program[GLOBAL_SHADER], "position");
	normal_loc = glGetAttribLocation(program[GLOBAL_SHADER], "normal");
	proj_loc = glGetUniformLocation(program[GLOBAL_SHADER], "proj_matrix");
	view_loc = glGetUniformLocation(program[GLOBAL_SHADER], "view_matrix");
	model_loc = glGetUniformLocation(program[GLOBAL_SHADER], "model_matrix");
	texture_loc = glGetUniformLocation(program[GLOBAL_SHADER], "tex");
	camera_pos_loc = glGetUniformLocation(program[GLOBAL_SHADER], "camera_pos");
	board_size_x_loc = glGetUniformLocation(program[GLOBAL_SHADER], "board_size_x");
	cube_coord_x_loc = glGetUniformLocation(program[GLOBAL_SHADER], "cube_coord_x");
	cube_coord_y_loc = glGetUniformLocation(program[GLOBAL_SHADER], "cube_coord_y");
	checkGlError(__FILE__, __LINE__);

	std::cerr << " Done" << std::endl;
}

void
Core::createImage(void)
{
	std::cerr << "Scene creation:";

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(VBOS_NUM, vbos);

	cube.generateModel();

	// MODEL buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, cube.model.v_mem_size, cube.model.v_array, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[MODEL_FACES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.model.f_mem_size, cube.model.f_array,	GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[MODEL_NORMALS]);
	glBufferData(GL_ARRAY_BUFFER, cube.model.n_mem_size, cube.model.n_array, GL_STATIC_DRAW);

	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();
	light.addNew();

	buildScene();

	// Link Attrib to Shader
	glEnableVertexAttribArray(position_loc);
	glEnableVertexAttribArray(normal_loc);
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	std::cerr << " Done" << std::endl;
}

void
Core::buildScene(void)
{
	// Room cube creation and placement
	cube.initMatrixArray();
	all_cubes_width = cube.space_btw_cube * (board.size_x - 1);
	cube.matrix_array[board.size].matrix.scale(all_cubes_width * 4, all_cubes_width * 4, all_cubes_width * 4);
    camera_ideal_pos = (all_cubes_width * 0.5 ) / tan(25.8) + 2.0f;

	camera.place(0.0f, 0.0f, 0.0f, 0.0f, camera_ideal_pos);

	light.changeBaseColor(MIDDLE, board.size_x * 0.7f * 1.5f, board.size_x * 0.7f * 1.5f, board.size_x * 0.7f * 1.5f);
    light.changeBaseColor(UP_MID, board.size_x * 0.7f * 0.16f, board.size_x * 0.7f * 0.7f, board.size_x * 0.7f * 0.06f);
    light.changeBaseColor(UP_LEFT, board.size_x * 0.7f * 0.3f, board.size_x * 0.7f * 0.2f, board.size_x * 0.7f * 0.1f);
    light.changeBaseColor(MID_LEFT, board.size_x * 0.7f * 0.7f, board.size_x * 0.7f * 0.31f, board.size_x * 0.7f * 0.09f);
    light.changeBaseColor(MID_RIGHT, board.size_x * 0.7f * 0.2f, board.size_x * 0.7f * 0.4f, board.size_x * 0.7f * 0.4f);
    light.changeBaseColor(DOWN_MID, board.size_x * 0.7f * 0.3f, board.size_x * 0.7f * 0.1f, board.size_x * 0.7f * 0.3f);
	light.changeBaseColor(UP_RIGHT, board.size_x * 0.7f * 0.1f, board.size_x * 0.7f * 0.6f, board.size_x * 0.7f * 0.2f);
	light.changeBaseColor(DOWN_RIGHT, board.size_x * 0.7f * 0.1f, board.size_x * 0.7f * 0.1f, board.size_x * 0.7f * 0.6f);
	light.changeBaseColor(DOWN_LEFT, board.size_x * 0.7f * 0.6f, board.size_x * 0.7f * 0.1f, board.size_x * 0.7f * 0.2f);

	light.changeProperty(MIDDLE, 100.0f, 0.1f, 0.71f);
	light.changeProperty(1, 50.0f, 0.01f, 0.01f);
	light.changeProperty(2, 50.0f, 0.01f, 0.01f);
	light.changeProperty(3, 50.0f, 0.01f, 0.01f);
	light.changeProperty(4, 50.0f, 0.01f, 0.01f);
	light.changeProperty(5, 50.0f, 0.01f, 0.01f);
	light.changeProperty(6, 50.0f, 0.01f, 0.01f);
	light.changeProperty(7, 50.0f, 0.01f, 0.01f);
	light.changeProperty(8, 50.0f, 0.01f, 0.01f);

	light.place(MIDDLE, 0.0f, 0.0f, all_cubes_width * 4);
    light.place(UP_LEFT, all_cubes_width * -1.80f, all_cubes_width * 1.80f, all_cubes_width);
    light.place(UP_MID, 0.0f,all_cubes_width * 1.72f, all_cubes_width);
    light.place(UP_RIGHT, all_cubes_width * 1.80f, all_cubes_width * 1.80f, all_cubes_width);
    light.place(MID_LEFT, all_cubes_width * -1.72f, 0.0f, all_cubes_width);
    light.place(MID_RIGHT, all_cubes_width * 1.72f, 0.0f, all_cubes_width);
    light.place(DOWN_LEFT, all_cubes_width * -1.80f, all_cubes_width * -1.80f, all_cubes_width);
    light.place(DOWN_MID, 0.0f,all_cubes_width * -1.72f, all_cubes_width);
    light.place(DOWN_RIGHT, all_cubes_width * 1.80f, all_cubes_width * -1.80f, all_cubes_width);

	light.applyAllTransformations();
	light.turnAdditionalOff();
	animator.init(&cube, &light);
	cube.applyTransformation();
}

void
Core::createAndLoadTextures(void)
{
	int		err = 0;

	std::cerr << "Texture loading:";

	glGenTextures(TEX_NUM, texture.tex);

	glActiveTexture(GL_TEXTURE0);
	texture.generateCustomTexture(texture.tex[ROOM_TEXTURE]);

	glActiveTexture(GL_TEXTURE1);
	err += texture.load2DTexture("./textures/bioshock_1.jpg", texture.tex[BIOSHOCK_TEXTURE]);

	glActiveTexture(GL_TEXTURE2);
	err += texture.load2DTexture("./textures/dark_souls_ban.jpg", texture.tex[DARK_SOULS_BAN_TEXTURE]);

	glActiveTexture(GL_TEXTURE3);
	err += texture.load2DTexture("./textures/artorias_0.jpg", texture.tex[ARTORIAS_TEXTURE]);

	glActiveTexture(GL_TEXTURE4);
	err += texture.load2DTexture("./textures/diablo_1.jpg", texture.tex[DIABLO_TEXTURE]);

    glActiveTexture(GL_TEXTURE5);
    err += texture.load2DTexture("./textures/courbet.jpg", texture.tex[COURBET_TEXTURE]);

    glActiveTexture(GL_TEXTURE6);
    err += texture.load2DTexture("./textures/dragon.jpg", texture.tex[DRAGON_TEXTURE]);

	if (err == 0)
		std::cerr << " Done" << std::endl;
	else
		std::cerr << err << " errors." << std::endl;
}

void
Core::resetAnimation(void)
{
	if (reset_stage == INITIATE)
	{
		//
		// Maybe finish each animation
		//
		// Force move animation to INITIATE.
		if (cube.applyTransformation() == 0.0f)
			reset_stage = PREP;
	}
	if (reset_stage == PREP)
	{
		move_stage = OFF;
		solv_stage = OFF;
		unsolv_stage = OFF;
        hyper_speed = 0;
        hyper_stage = NOTHING;
		game_mode = 0;
		game_stage = NOTHING;
		invisible_cube_toggle = SHOW;
		reset_stage = ANIM;
        animator.setDefaultValues();
	}
	if (reset_stage == ANIM)
	{
		reset_stage = animator.resetAnimation();
	}
	if (reset_stage == FINALIZE)
	{
		board.reset();
		if (shuffle_type == VALID)
        	board.validShuffle();
		else if (shuffle_type == RANDOM)
			board.randomShuffle();
		shuffle_type = NONE;
		cube.reset();
		resetLight();
		invisible_cube_toggle = HIDE;
		reset_stage = OFF;
	}
}

void
Core::lightAnimation(void)
{
	if (light_blink == 1)
	{
		animator.blinkLight();
	}
	if (light_move == 1)
	{
		if (light_bounce == 1)
		{
            for (int i = 1; i < light.getNumberOfLight(); i++)
            {
                animator.lightBounce(i, (int) (all_cubes_width * 42), 0.01f);
            }
        }
        else
        {
            for (int i = 1; i < light.getNumberOfLight(); i++)
            {
                animator.lightMoves(i, (int) (all_cubes_width * 37), 0.08f);
            }
        }
        if (light_rot == 0)
		{
            for (int i = 1; i < light.getNumberOfLight(); i++)
            {
                light.applyTransformation(i);
            }
		}
		if (light_rot == 1) {
			light.applyAllTransformations();
		}
	}
}

void
Core::moveAnimation(void)
{
	if (move_stage == INITIATE)
	{
		if (solv_stage == OFF && reset_stage == OFF && unsolv_stage == OFF)
        {
            game_mode = 0;
            game_stage = NOTHING;
            move_stage = PREP;
            animator.start_tick = animator.current_tick;
            animator.setDefaultValues();
        }
    }
    if (move_stage == PREP)
	{
        if (animator.tick_flow == 150 && board.size_x >= 10)
		{
            light.turnAdditionalOn();
            light_move = 1;
            light_bounce = 1;
            light.changeAllAdditionalCurrentColor(board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.33f);
        }
        else if (animator.tick_flow == 450 && board.size_x >= 10)
        {
            animator.setDefaultValues();
            light_bounce = 0;
            light.changeAllAdditionalCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        }
        else if (animator.tick_flow == 1000 + 0 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 20 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 40 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 60 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 80 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 100 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 120 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 140 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);
        else if (animator.tick_flow == 1000 + 160 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.43f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.23f);

        else if (animator.tick_flow == 1400 + 0 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 20 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 40 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 60 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 80 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 100 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 120 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 140 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);
        else if (animator.tick_flow == 1400 + 160 && board.size_x >= 10)
            light.changeNextCurrentColor(board.size_x * 0.7f * 0.23f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.43f);

        else if (animator.tick_flow == 2000 && board.size_x >= 10)
        {
            light.changeAllAdditionalCurrentColor(board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.33f, board.size_x * 0.7f * 0.33f);
        }
        move_stage = ANIM;
    }
    if (move_stage == ANIM)
	{
        if (hyper_stage == PLAY)
        {
            if (ultra_speed == 0)
                move_stage = cube.hyperSpeed();
            else
                move_stage = cube.hyperSpeed(board.size_x * 42);
        }
        else
        {
            move_stage = cube.createMoveAnimationFromSwapList();
            cube.applyTransformation();
        }
    }
    if (move_stage == FINALIZE)
	{
        if (cube.applyTransformation() == 0.0f && board.swap_queue_status == EMPTY)
		{
            resetLight();
            invisible_cube_toggle = SHOW;
            move_stage = OFF;
        }
    }
}

void
Core::solvAnimation(void)
{
	if (solv_stage == INITIATE)
	{
		if (move_stage == OFF && reset_stage == OFF && unsolv_stage == OFF)
		{
			game_mode = 0;
			game_stage = NOTHING;
            time_to_solve = 0.0f;
			solv_stage = PREP;
		}
    }
    if (solv_stage == PREP)
	{
        if (animator.tick_flow >= 60 && animator.tick_flow <= 120)
        {
            light.turnAdditionalOn();
        }
        if (animator.tick_flow >= 120 && animator.tick_flow <= 180)
        {
            light_move = 1;
        }
        if (animator.tick_flow >= 180 && animator.tick_flow <= 240)
        {
            light_blink = 1;
            light_rot = 1;
        }
		if (th == NULL)
			th = new std::thread(&Core::resolve, this);
        solv_stage = ANIM;
    }
    if (solv_stage == ANIM)
	{
        solv_stage = animator.solvAnimation();
    }
    if (solv_stage == FINALIZE)
	{
        if (board.swap_queue_status == FILLING)
		{
            solv_stage = INITIATE;
        }
		else if (board.swap_queue_status == READY)
		{
            th->join();
            delete th;
            th = NULL;
            resetLight();
            solv_stage = OFF;
            move_stage = INITIATE;
        }
		else if (board.swap_queue_status == UNSOLVABLE)
		{
            th->join();
            delete th;
            th = NULL;
            resetLight();
            solv_stage = OFF;
            unsolv_stage = INITIATE;
        }
		else
        {
            solv_stage = OFF;
        }
    }
}

void
Core::unsolvAnimation(void)
{
	if (unsolv_stage == INITIATE)
	{
		if (move_stage == OFF && reset_stage == OFF && solv_stage == OFF)
        {
            game_mode = 0;
            game_stage = NOTHING;
            unsolv_stage = ANIM;
            light.turnAdditionalOn();
        }
    }
    if (unsolv_stage == ANIM)
	{
        unsolv_stage = animator.unsolvAnimation();
    }
    if (unsolv_stage == FINALIZE)
	{
        light.turnAdditionalOff();
        unsolv_stage = OFF;
	}
}

void
Core::gameStageManager(void)
{
    if (solv_stage == OFF && unsolv_stage == OFF && move_stage == OFF && reset_stage == OFF)
    {
        if (game_mode == 1)
        {
            if (board.isItSolved())
            {
                game_mode = 0;
            }
            else if (game_stage == NOTHING)
            {
                game_stage = PLAY;
                game_mode = 0;
            }
            else if (game_stage == PLAY)
            {
                game_stage = STOP;
                game_mode = 0;
            }
        }
        if (game_stage == STOP)
        {
            reset_stage = INITIATE;
            shuffle_type = VALID;
            game_mode = 0;
            game_stage = NOTHING;
        }
        if (game_stage == PLAY)
        {
            // check if you win and show last piece.
            if (board.isItSolved())
            {
                invisible_cube_toggle = SHOW;
                light.turnAdditionalOn();
                light_move = 1;
                light_blink = 1;
                light_bounce = 1;
            }
        }
    }
}

void
Core::hyperStageManager(void)
{
    if (solv_stage == OFF && unsolv_stage == OFF && reset_stage == OFF)
    {
        if (hyper_speed == 1 && hyper_stage == NOTHING)
        {
            hyper_speed = 0;
            hyper_stage = STOP;
        }
        if (hyper_stage == STOP)
        {
            if (cube.applyTransformation() == 0.0f)
                hyper_stage = PLAY;
        }
        if (hyper_speed == 1 && hyper_stage == PLAY)
        {
            hyper_speed = 0;
            hyper_stage = NOTHING;
        }
    }
}

void
Core::updateEnvironment(int current_tick)
{
	animator.current_tick = current_tick;
    animator.tick_flow = animator.current_tick - animator.start_tick;
	if (pause == 0)
	{
        hyperStageManager();
        gameStageManager();
        resetAnimation();
        moveAnimation();
        solvAnimation();
        unsolvAnimation();
        lightAnimation();
	}
}

void
Core::resetLight(void)
{
	light.turnAdditionalOff();
	light_move = 0;
	light_blink = 0;
	light_rot = 0;
    light_bounce = 0;
	light.setDefaultValues();
	light.applyAllTransformations();
}

void
Core::resolve(void)
{
    double      start_time = 0.0f;
    double      end_time = 0.0f;

    start_time = glfwGetTime();
    board.swap_queue_status = FILLING;
    algo.setBoard(&board);
    algo.setHeuristic(heuristic);
    algo.resolve();
    end_time = glfwGetTime();
    time_to_solve = end_time - start_time;
}

void
Core::drawImage(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program[GLOBAL_SHADER]);
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj_matrix.val);
	glUniformMatrix4fv(view_loc, 1, GL_FALSE, camera.view.val);
	glUniform3f(camera_pos_loc, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform1i(glGetUniformLocation(program[GLOBAL_SHADER], "reverse_normal"), 0);

/*	struct Light
{
	vec3		position;
	vec3		color;
	float		shine;
	float		attenuation;
	float		ambient;
};
*/
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[0].position"), light.light_array[0].matrix.val[12], light.light_array[0].matrix.val[13], light.light_array[0].matrix.val[14]);	
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[0].color"), light.light_array[0].current_color.x, light.light_array[0].current_color.y, light.light_array[0].current_color.z);	
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[0].shine"), light.light_array[0].shine);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[0].attenuation"), light.light_array[0].attenuation);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[0].ambient"), light.light_array[0].ambient);

	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[1].position"), light.light_array[1].matrix.val[12], light.light_array[1].matrix.val[13], light.light_array[1].matrix.val[14]);	
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[1].color"), light.light_array[1].current_color.x, light.light_array[1].current_color.y, light.light_array[1].current_color.z);	
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[1].shine"), light.light_array[1].shine);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[1].attenuation"), light.light_array[1].attenuation);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[1].ambient"), light.light_array[1].ambient);

	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[2].position"), light.light_array[2].matrix.val[12], light.light_array[2].matrix.val[13], light.light_array[2].matrix.val[14]);	
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[2].color"), light.light_array[2].current_color.x, light.light_array[2].current_color.y, light.light_array[2].current_color.z);	
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[2].shine"), light.light_array[2].shine);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[2].attenuation"), light.light_array[2].attenuation);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[2].ambient"), light.light_array[2].ambient);

	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[3].position"), light.light_array[3].matrix.val[12], light.light_array[3].matrix.val[13], light.light_array[3].matrix.val[14]);	
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[3].color"), light.light_array[3].current_color.x, light.light_array[3].current_color.y, light.light_array[3].current_color.z);	
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[3].shine"), light.light_array[3].shine);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[3].attenuation"), light.light_array[3].attenuation);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[3].ambient"), light.light_array[3].ambient);

	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[4].position"), light.light_array[4].matrix.val[12], light.light_array[4].matrix.val[13], light.light_array[4].matrix.val[14]);	
	glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[4].color"), light.light_array[4].current_color.x, light.light_array[4].current_color.y, light.light_array[4].current_color.z);	
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[4].shine"), light.light_array[4].shine);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[4].attenuation"), light.light_array[4].attenuation);
	glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[4].ambient"), light.light_array[4].ambient);

	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[5].position"), light.light_array[5].matrix.val[12], light.light_array[5].matrix.val[13], light.light_array[5].matrix.val[14]);
	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[5].color"), light.light_array[5].current_color.x, light.light_array[5].current_color.y, light.light_array[5].current_color.z);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[5].shine"), light.light_array[5].shine);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[5].attenuation"), light.light_array[5].attenuation);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[5].ambient"), light.light_array[5].ambient);

	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[6].position"), light.light_array[6].matrix.val[12], light.light_array[6].matrix.val[13], light.light_array[6].matrix.val[14]);
	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[6].color"), light.light_array[6].current_color.x, light.light_array[6].current_color.y, light.light_array[6].current_color.z);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[6].shine"), light.light_array[6].shine);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[6].attenuation"), light.light_array[6].attenuation);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[6].ambient"), light.light_array[6].ambient);

	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[7].position"), light.light_array[7].matrix.val[12], light.light_array[7].matrix.val[13], light.light_array[7].matrix.val[14]);
	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[7].color"), light.light_array[7].current_color.x, light.light_array[7].current_color.y, light.light_array[7].current_color.z);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[7].shine"), light.light_array[7].shine);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[7].attenuation"), light.light_array[7].attenuation);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[7].ambient"), light.light_array[7].ambient);

	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[8].position"), light.light_array[8].matrix.val[12], light.light_array[8].matrix.val[13], light.light_array[8].matrix.val[14]);
	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[8].color"), light.light_array[8].current_color.x, light.light_array[8].current_color.y, light.light_array[8].current_color.z);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[8].shine"), light.light_array[8].shine);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[8].attenuation"), light.light_array[8].attenuation);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[8].ambient"), light.light_array[8].ambient);

	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[9].position"), light.light_array[9].matrix.val[12], light.light_array[9].matrix.val[13], light.light_array[9].matrix.val[14]);
	 glUniform3f(glGetUniformLocation(program[GLOBAL_SHADER], "light[9].color"), light.light_array[9].current_color.x, light.light_array[9].current_color.y, light.light_array[9].current_color.z);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[9].shine"), light.light_array[9].shine);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[9].attenuation"), light.light_array[9].attenuation);
	 glUniform1f(glGetUniformLocation(program[GLOBAL_SHADER], "light[9].ambient"), light.light_array[9].ambient);

	glBufferData(GL_ARRAY_BUFFER, cube.model.v_mem_size, cube.model.v_array, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.model.f_mem_size, cube.model.f_array, GL_STATIC_DRAW);

	// Draw Cubes
	glUniform1i(texture_loc, texture_selection);
	glUniform1f(board_size_x_loc, board.size_x);
	for (int i = 0; i < (board.size - invisible_cube_toggle); i++)
	{
		glUniform1f(cube_coord_x_loc, board.graph_cell[i].x);
		glUniform1f(cube_coord_y_loc, board.graph_cell[i].y);
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, cube.matrix_array[i].matrix.val);
		glDrawElements(GL_TRIANGLES, cube.model.f_num_elem, GL_UNSIGNED_BYTE, 0);
	}

	// Draw ROOM
	glUniform1i(texture_loc, ROOM_TEXTURE);
	glUniform1i(glGetUniformLocation(program[GLOBAL_SHADER], "reverse_normal"), 1);
	glUniformMatrix4fv(model_loc, 1, GL_FALSE, cube.matrix_array[board.size].matrix.val);
	glDrawElements(GL_TRIANGLES, cube.model.f_num_elem, GL_UNSIGNED_BYTE, 0);

	 checkGlError(__FILE__, __LINE__);
}


void
Core::checkGlError(std::string file, int line)
{
    GLenum        err;

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        if (err == GL_INVALID_ENUM)
			std::cerr << "GL: Invalid enum in " << file << " line " << line << std::endl;
        else if (err == GL_INVALID_VALUE)
			std::cerr << "GL: Invalid value in " << file << " line " << line << std::endl;
        else if (err == GL_INVALID_OPERATION)
			std::cerr << "GL: Invalid operation in " << file << " line " << line << std::endl;
        else if (err == GL_INVALID_FRAMEBUFFER_OPERATION)
			std::cerr << "GL: Invalid framebuffer operation in " << file << " line " << line << std::endl;
        else if (err == GL_OUT_OF_MEMORY)
			std::cerr << "GL: Out of memory in " << file << " line " << line << std::endl;
		exit(0);
    }
}
