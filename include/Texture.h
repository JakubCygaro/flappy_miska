#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <memory>
#include "Font.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include <format>

class Texture
{
private:
	int m_width;
	int m_height;
	std::shared_ptr<SDL_Texture*> m_texture;
	Texture(int h, int v, SDL_Texture*);
public:
	Texture();
	static Texture load_texture(SDL_Renderer*, const char*);
	static Texture load_from_font(SDL_Renderer* renderer, Font& font, const char *text, SDL_Color text_color);
	//Texture(Texture&&) noexcept;
	Texture(const Texture&) noexcept;
	~Texture();
	int get_width() const;
	int get_height() const;
	SDL_Texture* get_texture() const;
	Texture clone() const;
};


#endif
