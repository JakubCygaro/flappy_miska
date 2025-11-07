#pragma once

#include "Font.h"
#include "Texture.h"
#include <optional>
#include "Object.h"
#include <string>


class TextBox : public GameObject
{
private:
	Font m_font;
    SDL_Color m_color, m_background_color;
	Texture m_text_texture;
	std::string m_text;
public:
	TextBox(float x, float y, int w, int h, Font f, std::string text,  SDL_Renderer*);
	TextBox(const TextBox&) = delete;
	virtual ~TextBox();
	virtual void draw(SDL_Renderer*) const override;
	void set_background_texture(std::optional<Texture>);
	void set_font_color(SDL_Color);
	SDL_Color get_font_color() const;
	void set_background_color(SDL_Color);
	SDL_Color get_background_color() const;
	void set_text(std::string, SDL_Renderer*);
	std::string get_text() const;
	void set_font(Font, SDL_Renderer*);
	Font get_font() const;
};

