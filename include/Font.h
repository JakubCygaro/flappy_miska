#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>


class Font
{
private:
	std::shared_ptr<TTF_Font*> m_font;
	int m_size;
public:
	Font(const char* path, int size);
	Font(Font&&) noexcept;
	Font(const Font&) noexcept;
	Font& operator=(Font& other) noexcept;
	Font& operator=(Font&& other) noexcept;
	~Font();
	TTF_Font* get_font() const;
};

