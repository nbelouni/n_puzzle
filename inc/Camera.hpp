#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Mat4.hpp"
# include "Vec3.hpp"

class Camera
{
private:
	void					setView(void);
	void					set(void);

public:
	Camera(void);
	~Camera(void);

	Mat4<float>				view;
	Vec3<float>				pos;
	Vec3<float>				lookAt;
	Vec3<float>				forward;
	Vec3<float>				up;
	Vec3<float>				right;
	float					move_speed;
	float					cam_speed;
	float					hangle; //horizontal camera angle
	float					vangle; //vertical camera angle

	void					init(void);
	void					updateAngles(double &xpos, double &ypos, uint &res_x, uint &res_y);
	void					moveForward(void);
	void					place(float x_a, float y_a, float x_p, float y_p, float z_p);
	void					place(void);
	void					moveBackward(void);
	void					strafeRight(void);
	void					strafeLeft(void);
	void					rotate(void);
	void					printPos(void);
};

#endif