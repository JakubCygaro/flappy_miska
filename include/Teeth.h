#ifndef TEETH_H
#define TEETH_H

#include <SDL2/SDL.h>
#include "Texture.h"
#include "Vector2.h"
#include <SDL2/SDL.h>
#include "Texture.h"

class Teeth
{
private:
	Uint32 m_gap;
	Texture m_texture;
	Vector2 m_pos;
	int m_tooth_width;
	float m_gap_offset;
public:
	Teeth(SDL_Renderer* r, Uint32 gap, int tooth_width, Vector2 tooth_size, float gap_offset, Texture tooth_texture);
	void draw(SDL_Renderer* r, Vector2 screen_size);
	Vector2 get_pos() const;
	int get_width() const;
	void set_pos(Vector2);
	bool check_collision(SDL_FRect other, Vector2 screen_size);
};

#endif

