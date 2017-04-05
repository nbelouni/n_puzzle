
#include "Camera.hpp"

Camera::Camera(void)
{
}

Camera::~Camera(void)
{
}

void
Camera::setView(void)
{
	/*
	rx		ux		-dx		0
	ry		uy		-dy		0
	rz		uz		-dz		0
	0		0		0		1
	*/
	// first column
	view[0] = right.x;
	view[4] = right.y;
	view[8] = right.z;
	view[12] = 0.0f;
	// second column
	view[1] = up.x;
	view[5] = up.y;
	view[9] = up.z;
	view[13] = 0.0f;
	// third column
	view[2] = -forward.x;
	view[6] = -forward.y;
	view[10] = -forward.z;
	view[14] = 0.0f;
	// fourth column
	view[3] = 0.0f;
	view[7] = 0.0f;
	view[11] = 0.0f;
	view[15] = 1.0f;
}

void
Camera::set(void)
{
	Mat4<float>		translation;

	up.set(0.0f, 1.0f, 0.0f);
	right.crossProduct(forward, up);
	right.normalize();
	up.crossProduct(right, forward);
	up.normalize();
	setView();
	translation.setTranslation(0.0f - pos.x, 0.0f - pos.y, 0.0f - pos.z);
	view.multiply(translation);
}

void
Camera::place(float x_a, float y_a, float x_p, float y_p, float z_p)
{
	hangle = x_a;
	vangle = y_a;
	pos.set(x_p, y_p, z_p);
}

void
Camera::place(void)
{
	hangle = 0.0f;
	vangle = 0.0f;
	pos.set(0.0f, 0.0f, 0.0f);
}

void
Camera::init(void)
{
	hangle = 0.0f;
	vangle = 0.0f;
	move_speed = 0.1f;
	cam_speed = 0.2;
	pos.set(0.0f, 0.0f, 0.0f);
	lookAt.set(0.0f, 0.0f, -1.0f);
	forward.set(lookAt - pos);
	forward.normalize();
	set();
}

void
Camera::updateAngles(double &xpos, double &ypos, uint &res_x, uint &res_y)
{
	vangle -= ((ypos - res_y / 2) * cam_speed);
	if (vangle > 89)
		vangle = 89;
	if (vangle < -89)
		vangle = -89;
	hangle += ((xpos - res_x / 2) * cam_speed);
	hangle = fmod(hangle, 360);
}

void
Camera::moveForward(void)
{
	pos += forward * move_speed;
	set();
}

void
Camera::moveBackward(void)
{
	pos -= forward * move_speed;
	set();
}

void
Camera::strafeRight(void)
{
	pos += right * move_speed;
	set();
}

void
Camera::strafeLeft(void)
{
	pos -= right * move_speed;
	set();
}

void
Camera::rotate(void)
{
	float			hr;
	float			vr;

	hr = hangle * M_PI / 180;
	vr = vangle * M_PI / 180;
	forward.set(cos(vr) * sin(hr),
				sin(vr),
				cos(vr) * cos(hr) * -1);
	forward.normalize();
	set();
}

void
Camera::printPos(void)
{
	std::cout 	<< "(" << hangle
				<< "f, " << vangle
				<< "f, " << pos.x
				<< "f, " << pos.y
				<< "f, " << pos.z
				<< "f)" << std::endl;
}