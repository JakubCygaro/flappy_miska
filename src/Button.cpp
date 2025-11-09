#include "Button.h"


Button::Button(float x, float y, int w, int h, std::optional<Texture> texture, std::string text, Font font, SDL_Renderer *r): 
	GameObject{x, y, w, h, texture},
	m_text{text},
	m_font{font},
	m_back_color{ SDL_Color{} },
	m_outline_color{ SDL_Color{} },
	m_font_color{ SDL_Color{ 0x0, 0x0, 0x0, 0xFF } },
	m_text_texture{ Texture::load_from_font(r, font, text.c_str(), m_font_color)},
	m_active{ false }
{

}
Button::Button(float x, float y, Texture texture, std::string text, Font font, SDL_Renderer *r) :
	GameObject {x, y, texture},
	m_text{text},
	m_font{font},
	m_back_color{ SDL_Color{} },
	m_outline_color{ SDL_Color{} },
	m_font_color{ SDL_Color{ 0xFF, 0xFF, 0xFF, 0xFF } },
	m_text_texture{ Texture::load_from_font(r, font, text.c_str(), m_font_color) },
	m_active{ false }
{

}

void Button::draw(SDL_Renderer* renderer) const {
	auto rect = SDL_FRect {
		m_x, m_y,
		(float)m_width, (float)m_height
	};

	SDL_SetRenderDrawColor(renderer, m_back_color.r, m_back_color.g, m_back_color.b, m_back_color.a);
	SDL_RenderFillRectF(renderer, &rect);

	if (m_texture.has_value()) {
		SDL_RenderCopyF(renderer, m_texture.value().get_texture(), NULL, &rect);
	}

	Vector2 text_pos {
		(rect.x + rect.w / 2) - static_cast<float>(m_text_texture.get_width() / 2.),
		(rect.y + rect.h / 2) - static_cast<float>(m_text_texture.get_height() / 2.),
	};

	rect = {
		.x = text_pos.x, .y = text_pos.y,
		.w = (float)m_text_texture.get_width(), .h = (float)m_text_texture.get_height()
	};
	SDL_RenderCopyF(renderer, m_text_texture.get_texture(), NULL, &rect);

}
void Button::set_width(const float& w){
    m_width = w;
}
void Button::set_height(const float& h){
    m_height = h;
}

void Button::set_back_color(SDL_Color col) {
	m_back_color = col;
}

void Button::set_outline_color(SDL_Color col) {
	m_outline_color = col;
}

SDL_Color Button::get_back_color() const {
	return m_back_color;
}

SDL_Color Button::get_outline_color() const {
	return m_outline_color;
}

void Button::set_font_color(SDL_Color col, SDL_Renderer *r) {
	m_font_color = col;
	m_text_texture = Texture::load_from_font(r, m_font, m_text.c_str(), m_font_color);
}

SDL_Color Button::get_font_color() const {
	return m_font_color;
}

std::string Button::get_text() const {
	return m_text;
}

void Button::set_text(std::string text, SDL_Renderer *r) {
	m_text = text;
	m_text_texture = Texture::load_from_font(r, m_font, m_text.c_str(), m_font_color);
}

void Button::set_active_texture(Texture texture) {
	m_active_texture = texture;
}

void Button::active() {
	m_active = true;
}

void Button::inactive() {
	m_active = false;
}

bool Button::is_active() const {
	return m_active;
}

Button::~Button(){}
