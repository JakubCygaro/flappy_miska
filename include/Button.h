#pragma once

#include "Object.h"
#include "Font.h"

class Button : public GameObject
{
private:
	std::string m_text;
	Font m_font;
	SDL_Color m_back_color, m_outline_color, m_font_color;
	Texture m_text_texture;
	std::optional<Texture> m_active_texture;
	bool m_active;
public:
	Button(float x, float y, 
		int w, int h, 
		std::optional<Texture> inactive_texture, 
		std::string text, Font font, 
		SDL_Renderer *r);
	Button(float x, float y, 
		Texture inactive_texture, 
		std::string text, 
		Font font, 
		SDL_Renderer *r);
	Button(const Button&) = delete;
	virtual ~Button();
	void draw(SDL_Renderer*) const override;
	void set_back_color(SDL_Color);
	void set_outline_color(SDL_Color);
	SDL_Color get_back_color() const;
	SDL_Color get_outline_color() const;
	void set_font_color(SDL_Color, SDL_Renderer *r);
	SDL_Color get_font_color() const;
	void set_text(std::string, SDL_Renderer *r);
	std::string get_text() const;
	void set_active_texture(Texture);
	void active();
	void inactive();
	bool is_active() const;
};

