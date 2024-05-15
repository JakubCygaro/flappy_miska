#include "TextBox.h"

#include "Font.h"
#include "Texture.h"
#include <optional>
#include "Object.h"
#include "SDL.h"
#include <string>

TextBox::TextBox(float x, float y, int w, int h, Font f, std::string text, SDL_Renderer *renderer) :
	GameObject{ x, y, w, h, std::nullopt },
	m_font{ f },
	m_text { text },
	m_color{ SDL_Color { 0, 0, 0, 255 } },
	m_background_color { SDL_Color {0, 0, 0, 0} },
	m_text_texture{ Texture::load_from_font(renderer, m_font, text.c_str(), m_color) }
{

}

TextBox::~TextBox(){}

void TextBox::draw(SDL_Renderer *renderer) const {
	SDL_FRect rect = {
		m_x, m_y,
		m_width, m_height
	};
	SDL_SetRenderDrawColor(renderer, m_background_color.r, m_background_color.g, m_background_color.b, SDL_ALPHA_TRANSPARENT);
	//SDL_RenderFillRectF(renderer, &rect);

	if(m_texture.has_value())
		SDL_RenderCopyF(renderer, m_texture.value().get_texture(), NULL, &rect);

	/*rect.x = (((rect.x + rect.w) / 2) - m_text_texture.get_width() / 2);
	rect.y = (((rect.y + rect.h) / 2) - m_text_texture.get_height() / 2);
	rect.w = m_text_texture.get_width();
	rect.h = m_text_texture.get_height();*/
	SDL_RenderCopyF(renderer, m_text_texture.get_texture(), NULL, &rect);
}

void TextBox::set_background_texture(std::optional<Texture> text) {
	m_texture = text;
}

void TextBox::set_font_color(SDL_Color col) {
	m_color = col;
}

SDL_Color TextBox::get_font_color() const {
	return m_color;
}

void TextBox::set_background_color(SDL_Color col) {
	m_background_color = col;
}

SDL_Color TextBox::get_background_color() const {
	return m_background_color;
}

void TextBox::set_text(std::string s, SDL_Renderer *r) {
	m_text = s;
	m_text_texture = Texture::load_from_font(r, m_font, m_text.c_str(), m_color);
}

std::string TextBox::get_text() const {
	return m_text;
}

void TextBox::set_font(Font font, SDL_Renderer *r) {
	m_font = font;
	m_text_texture = Texture::load_from_font(r, m_font, m_text.c_str(), m_color);
}

Font TextBox::get_font() const {
	return m_font;
}
