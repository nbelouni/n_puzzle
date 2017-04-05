#include "Texture.hpp"

Texture::Texture(void)
{
}

Texture::~Texture(void)
{
}

int
Texture::loadCubemap(std::vector<const GLchar*> faces, GLuint textureID)
{
	int					width, height;
	unsigned char*		image;
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for(GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		if (image == NULL)
		{
			std::cerr << " Error loading Cube texture";
			return (1);
		}
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return (0);
}  

int
Texture::load2DTexture(const GLchar *path, GLuint textureID)
{	
	glBindTexture(GL_TEXTURE_2D, textureID);
	int					width, height;
	unsigned char*		image;

	image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == NULL)
	{
		std::cerr << " Error loading 2D texture";
		return (1);
	}
	glTexImage2D(GL_TEXTURE_2D, 0,	GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return (0);
}

void
Texture::generateCustomTexture(GLuint textureID)
{
	GLuint	i, j = 0;

	for (i = 0; i < TEX_WIDTH; i++)
	{
		for (j = 0; j < TEX_WIDTH; j++)
		{
				custom_texture[i][j][0] = (GLubyte) 20;
				custom_texture[i][j][1] = (GLubyte) 20;
				custom_texture[i][j][2] = (GLubyte) 20;
				custom_texture[i][j][3] = (GLubyte) 255;				
		}
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, custom_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
}
