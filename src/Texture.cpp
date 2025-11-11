#include "Texture.h"

Texture Texture::load_texture(SDL_Renderer* renderer, const char* path)
{
    SDL_Texture* new_texture = NULL;
    SDL_Surface* loaded = IMG_Load(path);
    if (loaded == NULL) {
        throw std::runtime_error(std::format("could not load image `{}` . {}", path, IMG_GetError()));
    }
    new_texture = SDL_CreateTextureFromSurface(renderer, loaded);
    if (new_texture == NULL) {
        throw std::runtime_error(std::format("could not make texture from image `{}` . {}", path, IMG_GetError()));
    }
    Texture ret = Texture(loaded->h, loaded->w, new_texture);

    SDL_FreeSurface(loaded);

    return ret;
}

Texture Texture::load_from_font(SDL_Renderer* renderer, Font& font, const char* text, SDL_Color text_color)
{
    SDL_Surface* text_surface = TTF_RenderText_Solid(font.get_font(), text, text_color);
    if (text_surface == NULL) {
        throw std::runtime_error(std::format("could not render text surface. {}\n", TTF_GetError()));
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (texture == NULL) {
        throw std::runtime_error(std::format("could not render text texture. {}\n", SDL_GetError()));
    }

    Texture ret(text_surface->h, text_surface->w, texture);

    SDL_FreeSurface(text_surface);
    return ret;
}

Texture::Texture(int h, int w, SDL_Texture* t)
    : m_width { w }
    , m_height { h }
    , m_texture { std::make_shared<SDL_Texture*>(t) }
{
}

int Texture::get_height() const
{
    return this->m_height;
}
int Texture::get_width() const
{
    return this->m_width;
}
void Texture::set_width(const int& w) {
    this->m_width = w;
}
void Texture::set_height(const int& h) {
    this->m_height = h;
}

SDL_Texture* Texture::get_texture() const
{
    return *this->m_texture;
}

Texture::~Texture()
{
    if (this->m_texture.use_count() == 1 && (*this->m_texture) != NULL) {
        SDL_DestroyTexture(*this->m_texture);
        this->m_height = 0;
        this->m_width = 0;
        *this->m_texture = NULL;
    }
}

Texture Texture::clone() const
{
    return Texture(m_height, m_width, *m_texture);
}

Texture::Texture(const Texture& other) noexcept
    : m_width { other.m_width }
    , m_height { other.m_height }
    , m_texture { other.m_texture }
{
}
Texture& Texture::operator=(const Texture& other) noexcept
{
    m_height = other.m_height;
    m_width = other.m_width;
    m_texture = other.m_texture;
    return *this;
}
Texture::Texture()
    : m_width { 0 }
    , m_height { 0 }
    , m_texture { NULL }
{
}
