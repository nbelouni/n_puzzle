#include "Animator.hpp"

Animator::Animator(void)
{
	return ;
}

Animator::~Animator(void)
{
	return ;
}

void
Animator::init(Cube *cube, Light *light)
{
    start_tick = 0;
    current_tick = 0;
    tick_flow = 0;
	this->cube = cube;
	this->light = light;
	setDefaultValues();
}

void
Animator::setDefaultValues(void)
{
	light_moves_tick = -1;
	light_bounce_tick = -1;
	solv_tick = 0.0f;
	unsolv_tick = 0.0f;
	reset_tick = 0.0f;
	inc = 1.0f;
}

void
Animator::blinkLight(void)
{
	for (int i = 1; i < light->getNumberOfLight(); i++)
	{
		if (current_tick % (i * 5) == 0)
		{
			light->blink(i);
		}
	}
}

void
Animator::lightBounce(int light_id, int range, float speed)
{
	if (light_bounce_tick == -1)
	{
		light_bounce_tick = range;
	}
	if (light_bounce_tick > 0)
	{
		light->light_array[light_id].anim_transf.setIdentity();
		light->light_array[light_id].anim_transf.translate(-speed * (range - light_bounce_tick), 0.0f, 0.0f);
		light_bounce_tick -= inc;
	}
	if (light_bounce_tick == 0)
	{
		inc *= -1;
		light_bounce_tick = 1;
	}
	if (light_bounce_tick >= range)
	{
		inc *= -1;
	}
}

void
Animator::lightMoves(int light_id, int range, float speed)
{
	if (light_moves_tick == -1)
	{
		light_moves_tick = range;
	}
	if (light_moves_tick > 0)
	{
		light->light_array[light_id].anim_transf.setIdentity();
		light->light_array[light_id].anim_transf.translate(0.0f, 0.0f, -speed * (range - light_moves_tick));
		light_moves_tick -= inc;
	}
	if (light_moves_tick == 0)
	{
		inc *= -1;
		light_moves_tick = 1;
	}
	if (light_moves_tick >= range)
	{
		inc *= -1;
	}
}

int
Animator::solvAnimation(void)
{
	#define S_START		0.0f
	#define S_INIT		0.8f

	#define	S_MID		2.0f
	#define	S_FMID		3.2f

	#define S_FINAL		4.4f
	#define S_END		5.6f
	#define S_PAUSE		7.0f

	if (solv_tick >= S_PAUSE)
	{
		solv_tick = 0.01f;
		return (FINALIZE);
	}
	for (int i = 0; i < cube->board->size; i++)
	{
		if (solv_tick >= S_START && solv_tick < S_INIT)
			cube->matrix_array[i].anim_transf.scale(0.982, 0.982, 0.982);

		if (solv_tick >= S_INIT && solv_tick < S_MID)
			cube->matrix_array[i].anim_transf.rotate(3.0f, 0.0f, 0.0f, 1.0f);

		if (solv_tick >= S_MID && solv_tick < S_FMID)
			cube->matrix_array[i].anim_transf.rotate(-6.0f, 0.0f, 0.0f, 1.0f);

		if (solv_tick >= S_FMID && solv_tick < S_FINAL)
			cube->matrix_array[i].anim_transf.rotate(3.0f, 0.0f, 0.0f, 1.0f);

		if (solv_tick >= S_FINAL && solv_tick < S_END)
			cube->matrix_array[i].anim_transf.scale(1.018, 1.018, 1.018);

        if (solv_tick >= S_END && solv_tick <= S_END + 0.1f)
            cube->matrix_array[i].anim_transf.setIdentity();
    }
	solv_tick += 0.1f;
	cube->applyTransformation();
	return (ANIM);
}

int
Animator::unsolvAnimation(void)
{
	#define U_START		0.0f
	#define U_INIT		0.8f

	#define	U_MID		2.0f
	#define	U_FMID		3.2f
	#define	U_FINAL		4.4f

	#define U_END		40.0f
	#define U_PAUSE		42.0f

	if (unsolv_tick >= U_PAUSE)
	{
		unsolv_tick = 0.01f;
		return (FINALIZE);
	}
	for (int i = 0; i < cube->board->size; i++)
	{
		if (unsolv_tick >= U_START && unsolv_tick < U_INIT)
			cube->matrix_array[i].anim_transf.scale(0.982, 0.982, 0.982);

		if (unsolv_tick >= U_INIT && unsolv_tick < U_MID)
			cube->matrix_array[i].anim_transf.rotate(3.0f, 0.0f, 0.0f, 1.0f);

		if (unsolv_tick >= U_MID && unsolv_tick < U_FMID)
			cube->matrix_array[i].anim_transf.rotate(-3.0f, 0.0f, 0.0f, 1.0f);

		if (unsolv_tick >= U_FMID && unsolv_tick < U_FINAL)
		{
			cube->matrix_array[i].anim_transf.rotate(3.0f, 0.0f, 0.0f, 1.0f);
			cube->matrix_array[rand() % cube->board->size].anim_transf.rotate(30.0f, 0.0f, 1.0f, 0.0f);
		}
		if (unsolv_tick >= U_END && unsolv_tick <= U_END + 0.1f)
			cube->matrix_array[i].anim_transf.setIdentity();
	}
	unsolv_tick += 0.1f;
	cube->applyTransformation();
	return (ANIM);
}


int
Animator::resetAnimation(void)
{
	#define R_END		10.0f
	#define R_START		0.0f
	#define	R_MID		(R_END / 2.0f)

	if (reset_tick >= R_END)
	{
		reset_tick = 0.01f;
		return (FINALIZE);
	}
	for (int i = 0; i < cube->board->size; i++)
	{
		if (reset_tick >= 1.0f && reset_tick <= R_END)
			cube->matrix_array[i].move.rotate(reset_tick * 0.2f, 0, 1, 0);
		if (reset_tick >= R_START && reset_tick <= R_MID - 2.0f)
			cube->matrix_array[i].anim_transf.scale(0.992, 0.992, 0.992);
		if (reset_tick >= R_MID + 2.0f && reset_tick <= R_END)
			cube->matrix_array[i].anim_transf.scale(1.008, 1.008, 1.008);
	}
	reset_tick += 0.1f;
	cube->applyTransformation();
	return (ANIM);
}
