#include "Teeth.h"
#include <SDL.h>
#include "Texture.h"
#include "Vector2.h"


Teeth::Teeth(SDL_Renderer* renderer, Uint32 gap, int tooth_width, Vector2 pos, float gap_offset, Texture tooth_texture) :
	m_gap { gap },
	m_texture { tooth_texture },
	m_tooth_width { tooth_width },
	m_pos { pos },
	m_gap_offset { gap_offset }
{

}

void Teeth::draw(SDL_Renderer *renderer, Vector2 screen_size) {

	auto total_tooth_height = (screen_size.y - m_gap);

	float top_height = m_gap_offset > 0 ? 
		m_gap_offset * total_tooth_height : //you get more
		(1.0 + m_gap_offset) * total_tooth_height; // you get less

	float bottom_height = total_tooth_height - top_height; // you get the rest

	SDL_FRect rect{
		m_pos.x, m_pos.y,
		(float)m_tooth_width, top_height
	};

	// the top one
	SDL_RenderCopyExF(renderer,
		m_texture.get_texture(),
		NULL,
		&rect,
		0,
		NULL,
		SDL_FLIP_VERTICAL);

	//add the gap between both teeth
	rect.y += top_height + m_gap;
	rect.h = bottom_height;

	//the bottom one
	SDL_RenderCopyF(renderer, m_texture.get_texture(), NULL, &rect);
}
//i love repeating code
bool Teeth::check_collision(SDL_FRect other, Vector2 screen_size) {
	auto total_teeth_height = (screen_size.y - m_gap);

	float top_height = m_gap_offset > 0 ?
		m_gap_offset * total_teeth_height : //you get more
		(1.0 + m_gap_offset) * total_teeth_height; // you get less

	float bottom_height = total_teeth_height - top_height; // you get the rest

	SDL_FRect rect {
		m_pos.x, m_pos.y,
		(float)m_tooth_width, top_height
	};

	if (((other.x >= rect.x && other.x <= rect.x + rect.w) &&
		other.y <= rect.y + rect.h) || 
		((other.x + other.w >= rect.x && other.x + other.w <= rect.x + rect.w) &&
			other.y <= rect.y + rect.h))
		return true;
	
	rect.y += top_height + m_gap;
	rect.h = bottom_height;

	return ((other.x >= rect.x && other.x <= rect.x + rect.w) &&
		other.y >= rect.y) ||
		((other.x + other.w >= rect.x && other.x + other.w <= rect.x + rect.w) &&
			other.y >= rect.y);

}

Vector2 Teeth::get_pos() const {
	return m_pos;
}

int Teeth::get_width() const {
	return m_tooth_width;
}

void Teeth::set_pos(Vector2 new_pos) {
	m_pos = new_pos;
}
