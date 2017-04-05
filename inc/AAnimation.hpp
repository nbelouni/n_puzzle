#ifndef AANIMATION_HPP
# define AANIMATION_HPP

#include "Mat4.hpp"

class AAnimation
{

public:

	Mat4<float>		*matrix_array[];
	int				matrix_array_size;

	AAnimation();
	~AAnimation();

	
};

#endif