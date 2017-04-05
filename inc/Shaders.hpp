#ifndef SHADERS_HPP
# define SHADERS_HPP

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

# include <iostream>
# include <fstream>
# include <string>
# include <vector>
# include <glfw3.h>

class Shaders
{

public:
	GLuint		vertex_shader;
	GLuint		fragment_shader;

	Shaders(void);
	Shaders(Shaders const &src);
	~Shaders(void);

	void		load(const GLchar *vertex_shader, const GLchar *fragment_shader);
	bool		loadShader(GLenum type, std::string file_name, GLuint *current_shader);

	void		compile(void);
	bool		compileShader(GLuint *current_shader);

	bool		linkProgram(GLuint *program);

	Shaders		&operator=(Shaders const &rhs);
};

#endif
