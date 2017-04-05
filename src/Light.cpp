#include "Light.hpp"

Light::Light(void)
{
	_number_of_light = 0;
	return ;
}

Light::~Light(void)
{
	return ;
}

void
Light::setDefaultValues(void)
{
	rot_yolo = 0.0f;
	rot_yolo_count = 1.0f;
	rot_yolo_decr = 1.0f;
    current_light_id = 0;
}

void
Light::resetMatrices(void)
{
    for (int i = 0; i < _number_of_light; i++)
    {
        light_array[i].matrix.setIdentity();
        light_array[i].anim_transf.setIdentity();
    }
}

int
Light::addNew(float r, float g, float b, float s, float att, float amb)
{
	if (_number_of_light <= (MAX_LIGHTS - 1))
	{
		light_array[_number_of_light].matrix.setIdentity();
		light_array[_number_of_light].init_transf.setIdentity();
		light_array[_number_of_light].anim_transf.setIdentity();
		light_array[_number_of_light].base_color.set(r, g, b);
		light_array[_number_of_light].blink_color.set(r * 0.1, g * 0.1, b * 0.1);
		light_array[_number_of_light].current_color = light_array[_number_of_light].base_color;
		light_array[_number_of_light].color_state = BASE;
		light_array[_number_of_light].shine = s;
		light_array[_number_of_light].attenuation = att;
		light_array[_number_of_light].ambient = amb;
		_number_of_light++;
	}
	else
	{
		std::cerr << "MAX_LIGHTS reached, light not created." << std::endl;
	}
	// Return light index in light_array.
	return (_number_of_light - 1);
}

void
Light::applyAllTransformations(void)
{
	if (rot_yolo_count > 0.0f && rot_yolo_count < 20.0f)
		rot_yolo_count += 0.4f;
	if (rot_yolo_count < 0.0f && rot_yolo_count > -20.0f)
		rot_yolo_count -= 0.4f;

	if (rot_yolo_count > 20.0f)
		rot_yolo_decr += 0.2f;
	if (rot_yolo_count < -20.0f)
		rot_yolo_decr -= 0.2f;

	if (rot_yolo >= 500.0f && rot_yolo_count > 0.0f)
	{
		rot_yolo_count = -1.0f;
		rot_yolo_decr = -1.0f;
	}
	if (rot_yolo <= -500.0f && rot_yolo_count < 0.0f)
	{
		rot_yolo_count = 1.0f;
		rot_yolo_decr = 1.0f;
	}
	rot_yolo += rot_yolo_count - rot_yolo_decr;

	for (int i = 0; i < _number_of_light; i++)
	{
		light_array[i].matrix.setIdentity();
		light_array[i].matrix.rotate(rot_yolo, 0, 0, 1);
		light_array[i].matrix.multiply(light_array[i].init_transf);
		light_array[i].matrix.multiply(light_array[i].anim_transf);
	}
}

void
Light::applyTransformation(int light_id)
{
	light_array[light_id].matrix.setIdentity();
	light_array[light_id].matrix.multiply(light_array[light_id].init_transf);
	light_array[light_id].matrix.multiply(light_array[light_id].anim_transf);
}

void
Light::place(int light_id, float x, float y, float z)
{
	if (light_id < MAX_LIGHTS && light_id >= 0)
	{
		light_array[light_id].init_transf.setIdentity();
		light_array[light_id].init_transf.translate(x, y, z);
	}
}

void
Light::changeBaseColor(int light_id, float r, float g, float b)
{
	if (light_id < MAX_LIGHTS && light_id >= 0)
	{
		light_array[light_id].base_color.set(r, g, b);
		if (light_array[light_id].color_state == BASE)
			light_array[light_id].current_color = light_array[light_id].base_color;
	}
}

void
Light::changeProperty(int light_id, float s, float att, float amb)
{
	if (light_id < MAX_LIGHTS && light_id >= 0)
	{
		light_array[light_id].shine = s;
		light_array[light_id].attenuation = att;
		light_array[light_id].ambient = amb;
	}
}

void
Light::changeBlinkColor(int light_id, float r, float g, float b)
{
	if (light_id < MAX_LIGHTS && light_id >= 0)
	{
		light_array[light_id].blink_color.set(r, g, b);
		if (light_array[light_id].color_state == BLINK)
			light_array[light_id].current_color = light_array[light_id].blink_color;
	}
}

void
Light::blink(int light_id)
{
	if (light_id < MAX_LIGHTS && light_id >= 0)
	{
		if (light_array[light_id].color_state == BASE)
		{
			light_array[light_id].current_color = light_array[light_id].blink_color;		
			light_array[light_id].color_state = BLINK;
		}
		else if (light_array[light_id].color_state == BLINK)
		{
			light_array[light_id].current_color = light_array[light_id].base_color;		
			light_array[light_id].color_state = BASE;
		}
	}
}

void
Light::turnBlinkOff(void)
{
	for (int i = 0; i < _number_of_light; i++)
	{
		light_array[i].current_color = light_array[i].base_color;		
		light_array[i].color_state = BASE;
	}
}

void
Light::turnOn(int light_id)
{
	light_array[light_id].current_color = light_array[light_id].base_color;
	light_array[light_id].color_state = BASE;
}

void
Light::turnOff(int light_id)
{
	light_array[light_id].current_color.set(0,0,0);
	light_array[light_id].color_state = SHUT;
}

void
Light::turnAdditionalOn(void)
{
	turnOff(0);
	for (int i = 1; i < _number_of_light; i++)
	{
		turnOn(i);
	}
}

void
Light::turnAdditionalOff(void)
{
	turnOn(0);
	for (int i = 1; i < _number_of_light; i++)
	{
		turnOff(i);
	}
}

void
Light::changeAllAdditionalCurrentColor(float r, float g, float b)
{
    for (int i = 1; i < _number_of_light; i++)
    {
        light_array[i].current_color.x = r;
        light_array[i].current_color.y = g;
        light_array[i].current_color.z = b;
    }
}

void
Light::changeNextCurrentColor(float r, float g, float b)
{
    if (current_light_id == _number_of_light)
    {
        current_light_id = 1;
    }
    light_array[current_light_id].current_color.set(r, g, b);
    current_light_id++;
}

int
Light::getNumberOfLight(void) const
{
	return _number_of_light;
}