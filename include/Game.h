#pragma once
#include <map>
#include <vector>
#include <stack>
#include <random>
#include <stdexcept>
#include <format>
#include <string>
#include <optional>
#include <algorithm>
#include <chrono>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Object.h"
#include "Vector2.h"
#include "Teeth.h"
#include "Texture.h"
#include "Button.h"
#include "TextBox.h"

#define PLAY_BUTTON "play_button"
#define MENU_BUTTON "menu_button"
#define BUTTON_TEXTURE "btn_txt"
#define GROUND_TEXTURE "ground_txt"
#define SPACEBAR_TEXTURE "spc_txt"
#define TOOTH_TEXTURE "tooth_txt"
#define DEATH_SOUND "death_s"
#define DEBIL_SOUND "debil_s"
#define OOO_SOUND "ooo_s"
#define KOLEZANKO_SOUND "kolezanko_s"
#define GAGRI_SOUND "gagri_s"
#define TATA_SOUND "tata_s"
#define POLICJA_SOUND "policja_s"
#define BOLJER_SOUND "boljer_s"
#define KURWA_SOUND "kurwa_s"
#define RYSZARD_SOUND "ryszard_s"
#define JUMP_SOUND "jump_s"
#define MISKA "miska"
#define MISKA_JUMP -7
#define MISKA_FALL 1
#define MISKA_FALL_ACC 0.15
#define MISKA_HITBOX_SCALE .2
#define SPACEBAR_MESSAGE "spcbr_msg"
#define SCORE "score"
#define AUTHOR "author"
#define OBSTACLE_VEL -5
#define OBSTACLE_SPACING_SCALE 5
#define OBSTACLE_WIDTH_SCALE 1.1

struct KeyEventData {
	enum class Kind {
		KeyDown,
		KeyUp
	};
	const Kind kind;
	const SDL_Keysym keysym;
	KeyEventData(KeyEventData::Kind, SDL_Keysym);
};

struct MouseEventData {
	enum class Kind {
		ButtonDown,
		ButtonUp
	};
	const Kind kind;
	MouseEventData(MouseEventData::Kind);
};

enum class GameState {
	Menu,
	Readyup,
	Playing,
	Death
};

class Game
{
	using State = GameState;
private:
	int m_window_height, m_window_width;
	SDL_Renderer* m_renderer;
	SDL_Window* m_main_window;
	std::map<std::string, std::unique_ptr<GameObject>> m_game_objects;
	bool m_quit;
	std::optional<Texture> m_backgroud;
	Uint64 m_delta_time;
	const float m_fps;
	Vector2 m_miska_vel;
	std::vector<Teeth> m_obstacles;
	State m_game_state;
	float m_sin;
	std::vector<GameObject> m_ground;
	Uint32 m_score;
	bool m_score_flag;
	std::random_device m_rand_dev;
	std::mt19937 m_rng;
	std::uniform_real_distribution<float> m_dist;
	std::map<std::string, Texture> m_textures;
	Mix_Music* m_theme;
	std::map<std::string, Mix_Chunk*> m_sounds;

	void initialize_objects();
	void draw();
	std::optional<State> update();
	std::optional<Game::State> keyboard_input(KeyEventData);
	std::optional<Game::State> mouse_input(MouseEventData);

public:
	Game(int w, int h, float fps);
	void initialize();
	void run();
	void close();

};

