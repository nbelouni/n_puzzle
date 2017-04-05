#ifndef	TEXTURE_HPP
# define TEXTURE_HPP

# define TEX_NUM 7
# define TEX_WIDTH	32

# include <glfw3.h>
# include <iostream>
# include <vector>
# include "SOIL.h"

enum textures
{
	ROOM_TEXTURE,
	BIOSHOCK_TEXTURE,
	DARK_SOULS_BAN_TEXTURE,
	ARTORIAS_TEXTURE,
	DIABLO_TEXTURE,
	COURBET_TEXTURE,
	DRAGON_TEXTURE
};

class Texture
{

public:
	GLuint			tex[TEX_NUM];
	GLuint			number_of_texture;
	GLubyte			custom_texture[TEX_WIDTH][TEX_WIDTH][4];

	Texture(void);
	~Texture(void);

	int			load2DTexture(const GLchar *path, GLuint textureID);
	int			loadCubemap(std::vector<const GLchar*> faces, GLuint textureID);
	void		generateCustomTexture(GLuint textureID);
};

#endif