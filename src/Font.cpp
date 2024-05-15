#include "Font.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <format>

Font::Font(const char* path, int size) :
	m_size{size}
{
	TTF_Font* font = TTF_OpenFont(path, size);
	if (font == NULL) {
		throw std::runtime_error(std::format("Could not open font file `{}` . {}\n", path, TTF_GetError()));
	}
	//printf("%s\tNULL: %d\n", path, font == NULL);
	m_font = std::make_shared<TTF_Font*>(font);
	printf("Font\n");
}
Font::~Font() {
	if (m_font.use_count() == 1) {
		TTF_CloseFont(*m_font);
		*m_font = NULL;
		printf("closed font\n");
	}
	printf("~Font\n");
}

TTF_Font* Font::get_font() const {
	return *m_font;
}

Font::Font(Font&& other) noexcept :
	m_font{std::move(other.m_font)},
	m_size{other.m_size}
{
	m_size = 0;
	m_font = nullptr;
}
Font::Font(const Font& other) noexcept :
	m_font{ other.m_font },
	m_size{ other.m_size }
{

}
Font& Font::operator=(Font& other) noexcept {
	m_font = other.m_font;
	m_size = other.m_size;
	return *this;
}
Font& Font::operator=(Font&& other) noexcept {
	m_font = std::move(other.m_font);
	m_size = other.m_size;
	other.m_font = nullptr;
	other.m_size = 0;
	return *this;
}