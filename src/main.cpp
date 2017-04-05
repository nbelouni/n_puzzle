#include <iostream>
#include <string>
#include "../compiled_GLFW/glfw3.h"
#include "../inc/Core.hpp"

#ifndef __APPLE__
static void
glErrorCallback(GLenum        source,
				GLenum        type,
				GLuint        id,
				GLenum        severity,
				GLsizei       length,
				const GLchar* message,
				GLvoid*       userParam)
{
	(void)userParam;
	(void)length;
	std::cerr << "OpenGL Error:" << std::endl;
	std::cerr << "=============" << std::endl;
	std::cerr << " Object ID: " << id << std::endl;
	std::cerr << " Severity:  " << severity << std::endl;
	std::cerr << " Type:      " << type << std::endl;
	std::cerr << " Source:    " << source << std::endl;
	std::cerr << " Message:   " << message << std::endl;
	glFinish();
}
#endif

static void
keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Core		*core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));
	(void)scancode;
	(void)mods;
	(void)core;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (!core->th)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		core->pause = (core->pause == 0) ? 1 : 0;

    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        core->light.turnAdditionalOff();
    }
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        core->light.turnAdditionalOn();
    }

    // Magic size select, press Ctrl -> you number and release.
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
    {
        if (key == GLFW_KEY_KP_0 && action == GLFW_PRESS)
            core->sizeSelector(0);
        else if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
            core->sizeSelector(1);
        else if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
            core->sizeSelector(2);
        else if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
            core->sizeSelector(3);
        else if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
            core->sizeSelector(4);
        else if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
            core->sizeSelector(5);
        else if (key == GLFW_KEY_KP_6 && action == GLFW_PRESS)
            core->sizeSelector(6);
        else if (key == GLFW_KEY_KP_7 && action == GLFW_PRESS)
            core->sizeSelector(7);
        else if (key == GLFW_KEY_KP_8 && action == GLFW_PRESS)
            core->sizeSelector(8);
        else if (key == GLFW_KEY_KP_9 && action == GLFW_PRESS)
            core->sizeSelector(9);
    }
    if (core->size_select_active > 0 && key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE)
    {
        core->changeSize(core->size_select);
        core->size_select = 0;
        core->size_select_active = 0;
    }

    // Select Heuristic
    if (!glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
    {
        if (key == GLFW_KEY_KP_1 && action == GLFW_PRESS)
            core->heuristic = MANHATTAN1;
        else if (key == GLFW_KEY_KP_2 && action == GLFW_PRESS)
            core->heuristic = MANHATTAN2;
        else if (key == GLFW_KEY_KP_3 && action == GLFW_PRESS)
            core->heuristic = HAMMING;
        else if (key == GLFW_KEY_KP_4 && action == GLFW_PRESS)
            core->heuristic = PERMUTATION;
        else if (key == GLFW_KEY_KP_5 && action == GLFW_PRESS)
            core->heuristic = MATRIX;
    }

    // Cube move speed
	if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (core->pause == 0 && core->move_stage != OFF)
			core->cube.increaseMoveSpeed();
	}
	else if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (core->pause == 0 && core->move_stage != OFF)
			core->cube.decreaseMoveSpeed();
	}

	// Animations
	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		if (core->pause == 0 && core->reset_stage == OFF && !core->th)
        {
			core->shuffle_type = VALID;
            core->reset_stage = INITIATE;
        }
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (core->pause == 0 && core->reset_stage == OFF && !core->th)
        {
			core->shuffle_type = RANDOM;
            core->reset_stage = INITIATE;
        }
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
        if (core->pause == 0 && core->move_stage == OFF)
		{
            core->animator.start_tick = core->animator.current_tick;
            core->solv_stage = INITIATE;
        }
    }

    // Hyper Speed
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
        core->hyper_speed = (core->hyper_speed == 0) ? 1 : 0;
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        core->ultra_speed = (core->ultra_speed == 0) ? 1 : 0;

	// Game Mode
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		core->game_mode = (core->game_mode == 0) ? 1 : 0;
	if (core->game_stage == PLAY)
	{
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			core->board.findAndSwap(0);
			core->cube.initMatrixArray();
			core->cube.applyTransformation();
		}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			core->board.findAndSwap(1);
			core->cube.initMatrixArray();
			core->cube.applyTransformation();
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			core->board.findAndSwap(2);
			core->cube.initMatrixArray();
			core->cube.applyTransformation();
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			core->board.findAndSwap(3);
			core->cube.initMatrixArray();
			core->cube.applyTransformation();
		}
	}

    // Texture selection
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        core->texture_selection = BIOSHOCK_TEXTURE;
    else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        core->texture_selection = DARK_SOULS_BAN_TEXTURE;
    else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        core->texture_selection = ARTORIAS_TEXTURE;
    else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
        core->texture_selection = DIABLO_TEXTURE;
    else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
        core->texture_selection = DRAGON_TEXTURE;
    else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
        core->texture_selection = COURBET_TEXTURE;
}

static void
cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
{
	Core		*core = reinterpret_cast<Core *>(glfwGetWindowUserPointer(window));

	core->camera.updateAngles(xpos, ypos, core->res_x, core->res_y);
	glfwSetCursorPos(window, core->res_x / 2, core->res_y / 2);
}

void
updateCamera(Camera &camera, GLFWwindow *window)
{
	camera.rotate();
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveForward();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveBackward();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.strafeLeft();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.strafeRight();
}

static int
initGLFW(Core &core)
{
	if (!glfwInit())
		return (0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	core.window = glfwCreateWindow(core.res_x, core.res_y, "N-Puzzle", NULL, NULL);
	if (!core.window)
	{
		glfwTerminate();
		return (0);
	}
	glfwSetWindowUserPointer(core.window, &core);
	glfwMakeContextCurrent(core.window); // make the opengl context of the core.window current on the main thread
	glfwSwapInterval(1); // VSYNC 60 fps max
	glfwSetKeyCallback(core.window, keyCallback);
	glfwSetCursorPosCallback(core.window, cursorPosCallback);
	glfwSetInputMode(core.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //hide cursor
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

/*	// Set Transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/

	glEnable(GL_LINE_SMOOTH);

#ifndef __APPLE__
	if (glDebugMessageControlARB != NULL)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
		glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
		glDebugMessageCallbackARB((GLDEBUGPROCARB)glErrorCallback, NULL);
	}
#endif
	return (1);
}

void    printKeysUsage(void)
{
    std::cout << "Key Usage: " << std::endl;
    std::cout << std::endl;
    std::cout << "[ SPACE BAR ] -> Solve Puzzle" << std::endl;
    std::cout << "[ R ] -> Shuffle Randomly" << std::endl;
    std::cout << "[ V ] -> Shuffle from Resolved" << std::endl;
    std::cout << std::endl;
    std::cout << "[ + ] -> Increase Cube Speed" << std::endl;
    std::cout << "[ - ] -> Decrease Cube Speed" << std::endl;
    std::cout << "[ delete ] -> Hyper Speed" << std::endl;
    std::cout << "[ return ] -> Ultra Speed" << std::endl;
    std::cout << std::endl;
    std::cout << "[ WASD ] -> Move Camera" << std::endl;
    std::cout << "( Mouse ) -> Rotate Camera" << std::endl;
    std::cout << std::endl;
    std::cout << "[ 123456 ] -> Select Texture" << std::endl;
    std::cout << "[ Ctrl ] + [ NUMPAD 1234567890 ] then RELEASE [ Ctrl ]-> Change Board Size" << std::endl;
    std::cout << "[ NUMPAD 12345 ] -> Change Heuristic" << std::endl;
    std::cout << std::endl;
    std::cout << "[ G ] -> Gaming Mode" << std::endl;
    std::cout << "[ Arrows ] -> Play" << std::endl;
    std::cout << std::endl;
    std::cout << "[ esc ] -> Exit" << std::endl;
    std::cout << std::endl;
}

int main()
{
	Core			core;
	double			next_string_update, current_time, next_env_update;
	double			frame_counter = 0.0;
	double			tick_length = 1.0 / 60.0;
	int				current_tick = 0;
	std::string		title;

	std::srand(time(NULL));
    printKeysUsage();
	core.init();
	initGLFW(core);
	core.setupMatrices();
	core.genShaders();
	core.createImage();
	core.createAndLoadTextures();

	next_string_update = next_env_update = glfwGetTime();
	while (!glfwWindowShouldClose(core.window))
	{
		current_time = glfwGetTime();
		if (current_time >= 1.0)
		{
			glfwPollEvents();
			if (current_time - next_env_update >= tick_length)
			{
				next_env_update += tick_length;
				current_tick += 1;
				core.updateEnvironment(current_tick);
			}
			updateCamera(core.camera, core.window);
			core.drawImage();
			glfwSwapBuffers(core.window);
			frame_counter += 1.0;
		}
        if (current_time - next_string_update >= 1.0)
        {
            next_string_update += 1.0;
            title.clear();
            title.append("FPS: ");
            title.append(std::to_string((int)frame_counter));
            title.append(" - ");
            title.append("Board Size: ");
            title.append(std::to_string(core.board.size_x));
            title.append(" * ");
            title.append(std::to_string(core.board.size_y));
            title.append(" - ");
            if (core.pause == 1)
            {
                title.append(" ");
                title.append("PAUSE");
                title.append(" - ");
            }
            if (core.game_stage == PLAY)
            {
                title.append(" ");
                title.append("GAME MODE");
                title.append(" - ");
            }
            title.append("Heuristic:");
            if (core.heuristic == MANHATTAN1)
            {
                title.append(" ");
                title.append("MANHATTAN1");
                title.append(" - ");
            } else if (core.heuristic == MANHATTAN2)
            {
                title.append(" ");
                title.append("MANHATTAN2");
                title.append(" - ");
            } else if (core.heuristic == HAMMING)
            {
                title.append(" ");
                title.append("HAMMING");
                title.append(" - ");
            } else if (core.heuristic == PERMUTATION)
            {
                title.append(" ");
                title.append("PERMUATION");
                title.append(" - ");
            } else if (core.heuristic == MATRIX)
            {
                title.append(" ");
                title.append("MATRIX");
                title.append(" - ");
            }
            title.append("Time to Solve: ");
            if (core.time_to_solve != 0.0)
            {
                title.append(std::to_string(core.time_to_solve));
                title.append(" ");
            }
            glfwSetWindowTitle(core.window, title.c_str());
			frame_counter = 0.0;
		}
	}
	return 0;
}
