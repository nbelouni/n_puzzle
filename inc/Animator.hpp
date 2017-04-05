#ifndef ANIMATOR_HPP
# define ANIMATOR_HPP

# include "Light.hpp"
# include "Cube.hpp"

enum animation_stages
{
	OFF,
	INITIATE,
	PREP,
	ANIM,
	FINALIZE
};

class Animator
{

public:

	int			start_tick;
	int			current_tick;
    int         tick_flow;
	Cube		*cube;
	Light		*light;

	int			light_moves_tick;
    int         light_bounce_tick;

	float		solv_tick;
	float		unsolv_tick;
	float		reset_tick;

	int 		inc;

	Animator();
	~Animator();
	
	void		init(Cube *cube, Light *light);
	void		setDefaultValues();
	void		blinkLight(void);
	void		lightMoves(int light_id, int range, float speed);
	void		lightBounce(int light_id, int range, float speed);
	int			solvAnimation(void);
	int			unsolvAnimation(void);
	int			resetAnimation(void);

};

#endif