#ifndef LIGHT_HPP
# define LIGHT_HPP

# include <glfw3.h>
# include <vector>
# include "Vec3.hpp"
# include "Mat4.hpp"

# define MAX_LIGHTS		10

enum color_states
{
	BASE,
	BLINK,
	SHUT
};

enum color_places
{
    MIDDLE,
    UP_LEFT,
    UP_MID,
    UP_RIGHT,
    MID_LEFT,
    MID_RIGHT,
    DOWN_LEFT,
    DOWN_MID,
    DOWN_RIGHT
};

class Light
{

private:
	int		_number_of_light;

public:

	struct SimpleLight
	{
		Mat4<GLfloat>	matrix;
		Mat4<GLfloat>	init_transf;
		Mat4<GLfloat>	anim_transf;

		Vec3<GLfloat>	current_color;
		Vec3<GLfloat>	base_color;
		Vec3<GLfloat>	blink_color;
		int				color_state;

		float			shine;
		float			attenuation;
		float			ambient;
		
	};

	SimpleLight		light_array[MAX_LIGHTS];

	float			rot_yolo = 0.0f;
	float			rot_yolo_count = 1.0f;
	float			rot_yolo_decr = 1.0f;

    int             current_light_id = 1;

	Light(void);
	~Light(void);

	void		setDefaultValues(void);
    void        resetMatrices(void);
    int			addNew(float r = 1.0, float g = 1.0, float b = 1.0, float s = 50.0f, float att = 0.28f, float amb = 0.02f);
	void		applyAllTransformations(void);
	void		applyTransformation(int light_id);
	void		place(int light_id, float x, float y, float z);
	void		changeBaseColor(int light_id, float r, float g, float b);
	void		changeProperty(int light_id, float s, float att, float amb);
	void		changeBlinkColor(int light_id, float r, float g, float b);
	void		blink(int light_id);
	void		turnBlinkOff(void);
	void		turnOn(int light_id);
	void		turnOff(int light_id);
	void		turnAdditionalOn(void);
	void		turnAdditionalOff(void);
    void        changeAllAdditionalCurrentColor(float r, float g, float b);
    void        changeNextCurrentColor(float r, float g, float b);
	int			getNumberOfLight(void) const;
};

#endif