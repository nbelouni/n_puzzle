#include "Shaders.hpp"

Shaders::Shaders(void)
{
	return;
}

Shaders::Shaders(Shaders const &src)
{
	*this = src;
	return;
}

Shaders::~Shaders(void)
{
	return;
}

void
Shaders::load(const GLchar *vertex_shader, const GLchar *fragment_shader)
{
	if (loadShader(GL_VERTEX_SHADER, vertex_shader, &this->vertex_shader) == false)
	{
		std::cerr << "Vertex shader failed to load." << std::endl;
	}
	if (loadShader(GL_FRAGMENT_SHADER, fragment_shader, &this->fragment_shader) == false)
	{
		std::cerr << "Fragment shader failed to load." << std::endl;
	}
}

bool
Shaders::loadShader(GLenum type, std::string file_name, GLuint *current_shader)
{
	std::ifstream		source_file(file_name.c_str());
	std::string			shader_string;
	const GLchar		*shader_source;

	if (!source_file)
		return (false);

	shader_string.assign(std::istreambuf_iterator<char>(source_file),
						 std::istreambuf_iterator<char>());
	*current_shader = glCreateShader(type);

	shader_source = shader_string.c_str();
	glShaderSource(*current_shader, 1, (const GLchar**)&shader_source, NULL);

	return (true);
}

void
Shaders::compile(void)
{
	if (compileShader(&this->vertex_shader) == false)
		std::cerr << "Vertex shader failed to compile." << std::endl;
	if (compileShader(&this->fragment_shader) == false)
		std::cerr << "Fragment shader failed to compile." << std::endl;
}

bool
Shaders::compileShader(GLuint *current_shader)
{
	GLint		is_compiled;

	is_compiled = 0;
	glCompileShader(*current_shader);
	glGetShaderiv(*current_shader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == GL_FALSE)
	{
		GLint	max_length = 0;
		glGetShaderiv(*current_shader, GL_INFO_LOG_LENGTH, &max_length);
		std::vector<GLchar> comp_log(max_length);
		glGetShaderInfoLog(*current_shader, max_length, &max_length, &comp_log[0]);
		for (std::vector<char>::const_iterator i = comp_log.begin() ; i != comp_log.end() ; ++i)
		{
			std::cerr << *i ;
		}
		std::cerr << std::endl;
		glDeleteShader(*current_shader);
		return (false);
	}
	return (true);
}

bool
Shaders::linkProgram(GLuint *program)
{
	GLint		is_linked;

	is_linked = 0;
	glAttachShader(*program, this->vertex_shader);
	glAttachShader(*program, this->fragment_shader);
	glLinkProgram(*program);
	glGetProgramiv(*program, GL_LINK_STATUS, &is_linked);
	if (is_linked == GL_FALSE)
	{
		GLint	max_length = 0;
		glGetShaderiv(*program, GL_INFO_LOG_LENGTH, &max_length);
		std::vector<GLchar> link_log(max_length);
		glGetShaderInfoLog(*program, max_length, &max_length, &link_log[0]);
		for (std::vector<char>::const_iterator i = link_log.begin() ; i != link_log.end() ; ++i)
		{
			std::cerr << *i ;
		}
		std::cerr << std::endl;
		glDeleteProgram(*program);
		glDeleteShader(this->vertex_shader);
		glDeleteShader(this->fragment_shader);
		return (false);
	}
	glDetachShader(*program, this->vertex_shader);
	glDetachShader(*program, this->fragment_shader);
	return (true);
}

Shaders
&Shaders::operator=(Shaders const &rhs)
{
	this->vertex_shader = rhs.vertex_shader;
	this->fragment_shader = rhs.fragment_shader;
	return *this;
}
