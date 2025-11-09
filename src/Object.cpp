#include "Object.h"

GameObject::GameObject(float x, float y, int w, int h, std::optional<Texture> texture) :
	m_texture{texture},
    m_x{x},
    m_y{y},
    m_width{w},
    m_height{h},
	m_collison{(float)w, (float)h}
{

}
GameObject::GameObject(float x, float y, Texture texture) : 
    m_texture{texture},
	m_x{x},
	m_y{y},
	m_width{texture.get_width()},
	m_height{texture.get_height()},
	m_collison{ (float)texture.get_width(), (float)texture.get_height()}
{

}

//GameObject::GameObject(GameObject&& other) noexcept:
//	m_x{ other.m_x },
//	m_y{ other.m_y },
//	m_width{ other.m_width },
//	m_height{ other.m_height },
//	m_texture{ std::move(other.m_texture) },
//	m_collison{ other.m_collison }
//{
//	other.m_texture = std::nullopt;
//}
//
//GameObject::GameObject(const GameObject& other) noexcept :
//	m_x{ other.m_x },
//	m_y{ other.m_y },
//	m_width{ other.m_width },
//	m_height{ other.m_height },
//	m_texture{ other.m_texture },
//	m_collison{ other.m_collison }
//{
//
//}


void GameObject::draw(SDL_Renderer *renderer) const {
	if (!this->m_texture.has_value()) {
		return;
	}
	SDL_Rect rect {
		(int)this->m_x, (int)this->m_y,
		this->m_width, this->m_height
	};
	if (SDL_RenderCopy(renderer, this->m_texture.value().get_texture(), NULL, &rect)) {
		printf("could not draw object: %s\n", SDL_GetError());
	}
}

const std::optional<Texture>& GameObject::get_texture() const {
	return this->m_texture;
}

Vector2 GameObject::get_pos() const {
	return { this->m_x, this->m_y };
}
Vector2 GameObject::get_size() const {
	return { (float)this->m_width, (float)this->m_height };
}

void GameObject::set_pos(Vector2 new_pos) {
	this->m_x = new_pos.x;
	this->m_y = new_pos.y;
}

SDL_FRect GameObject::get_rect() const {
	return SDL_FRect{
		m_x, m_y,
		(float)m_width, (float)m_height
	};
}

/// <summary>
/// new_size.x = width
/// new_size.y = height
/// </summary>
/// <param name="new_size">new_size.x = width; new_size.y = height</param>
void GameObject::set_size(Vector2 new_size) {
	this->m_width = new_size.x;
	this->m_height = new_size.y;
}

GameObject::~GameObject(){}

SDL_FRect GameObject::get_collider() const {
	SDL_FRect ret {
		.x = static_cast<float>(m_x + m_width / 2.) - static_cast<float>(m_collison.x / 2.),
		.y = static_cast<float>(m_y + m_height / 2.) - static_cast<float>(m_collison.y / 2.),
		.w = m_collison.x, .h = m_collison.y
	};
	return ret;
}

void GameObject::set_collision_size(Vector2 sz) {
	m_collison = sz;
}
Vector2 GameObject::get_collision_size() const {
	return m_collison;
}
bool GameObject::point_collision(const GameObject& obj, const Vector2& point){
    auto rect = obj.get_rect();
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}
bool GameObject::point_collision(const GameObject* obj, const Vector2& point){
    auto rect = obj->get_rect();
    return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}
