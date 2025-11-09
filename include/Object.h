#pragma once
#include "Texture.h"
#include "optional"
#include <SDL2/SDL.h>
#include "Vector2.h"



class GameObject
{
protected:
	std::optional<Texture> m_texture;
	float m_x, m_y;
	int m_width, m_height;
	Vector2 m_collison;
public:
	GameObject(float x, float y, int w, int h, std::optional<Texture> texture);
	GameObject(float x, float y, Texture texture);
	virtual ~GameObject();
	virtual void draw(SDL_Renderer*) const;
	virtual const std::optional<Texture>& get_texture() const;
	virtual Vector2 get_pos() const;
	virtual void set_pos(Vector2 pos);
	virtual Vector2 get_size() const;
	virtual void set_size(Vector2 pos);
	virtual SDL_FRect get_rect() const;
	virtual SDL_FRect get_collider() const;
	virtual void set_collision_size(Vector2);
	virtual Vector2 get_collision_size() const;
    static bool point_collision(const GameObject& obj, const Vector2& point);
    static bool point_collision(const GameObject* obj, const Vector2& point);
};

