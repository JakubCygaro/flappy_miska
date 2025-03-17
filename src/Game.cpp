#include "Game.h"


using namespace std::literals::string_literals;

static Mix_Chunk* load_sound(const char* path) {
	auto sound = Mix_LoadWAV(path);
	if (sound == NULL) {
		throw std::runtime_error(std::format("could not load `{}`. {}\n", path, Mix_GetError()));
	}
	return sound;
}


Game::Game(int w, int h, float fps) :
	m_window_width{ w },
	m_window_height{ h },
	m_renderer{ NULL },
	m_main_window{ NULL },
	m_game_objects{ std::map<std::string, std::unique_ptr<GameObject>>() },
	m_quit{ false },
	m_delta_time{ 0 },
	m_fps{ fps },
	m_fps_cap{ 1000 / fps },
	m_miska_vel{ Vector2 { 0, 1 } },
	m_obstacles{ std::vector<Teeth>() },
	m_game_state{ State::Menu },
	m_sin{ 0.0 },
	m_ground{ std::vector<GameObject>() },
	m_score { 0 },
	m_score_flag { false },
	m_rand_dev { std::random_device() },
	m_rng { std::mt19937(m_rand_dev()) },
	m_dist { },
	m_textures { std::map<std::string, Texture>()},
	m_theme { NULL }
{
	auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
	m_rng.seed(seed);
}

void Game::initialize() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		throw std::runtime_error(std::format("could not initialize {}\n", SDL_GetError()));
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		printf("Warning: linear texture filtering not enabled\n");
	}

	SDL_DisplayMode display;
	if (SDL_GetDesktopDisplayMode(0, &display)){
		throw std::runtime_error("Could not determine the display size\n");
	}
	if (display.h < m_window_height)
		//m_window_height -= m_window_height - display.h;
		m_window_height = 0.85 * display.h;
	
	if (display.w < m_window_width)
		//m_window_width -= m_window_width - display.w;
		m_window_width = 0.67 * display.w;

	this->m_main_window = SDL_CreateWindow("Flappy Miska",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		this->m_window_width, this->m_window_height,
		SDL_WINDOW_SHOWN);

	if (this->m_main_window == NULL) {
		throw std::runtime_error(std::format("could not create main window {}\n", SDL_GetError()));
	}
	this->m_renderer = SDL_CreateRenderer(this->m_main_window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);

	if (this->m_renderer == NULL) {
		throw std::runtime_error(std::format("could not create renderer {}\n", SDL_GetError()));
	}
	SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x0, 0x0, 0xFF);

	int img_flags = IMG_INIT_PNG;

	if (!(IMG_Init(img_flags) & img_flags)) {
		throw std::runtime_error(std::format("could not initialize SDL_image. {}\n", IMG_GetError()));
	}

	if (TTF_Init() == -1) {
		throw std::runtime_error(std::format("could not initialize SDL_ttf. {}\n", TTF_GetError()));
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		throw std::runtime_error(std::format("could not initialize SDL_mixer. {}\n", Mix_GetError()));
	}

	initialize_objects();
}

void Game::initialize_objects() {

	// printf("loading textures\n");
	m_textures.emplace(MISKA, std::move(Texture::load_texture(m_renderer, "data/miska.png")));
	m_textures.emplace(BUTTON_TEXTURE, std::move(Texture::load_texture(m_renderer, "data/button.png")));
	m_textures.emplace(GROUND_TEXTURE, std::move(Texture::load_texture(m_renderer, "data/ground.png")));
	m_textures.emplace(SPACEBAR_TEXTURE, std::move(Texture::load_texture(m_renderer, "data/space.png")));
	m_textures.emplace(TOOTH_TEXTURE, std::move(Texture::load_texture(m_renderer, "data/zab.png")));

	// printf("loading music\n");
	m_sounds[DEATH_SOUND] = load_sound("data/death.wav");
	m_sounds[DEBIL_SOUND] = load_sound("data/debil.wav");
	m_sounds[OOO_SOUND] = load_sound("data/o_o_o.wav");
	m_sounds[KOLEZANKO_SOUND] = load_sound("data/kolezanko.wav");
	m_sounds[GAGRI_SOUND] = load_sound("data/gagri.wav");
	m_sounds[TATA_SOUND] = load_sound("data/tata.wav");
	m_sounds[POLICJA_SOUND] = load_sound("data/policja.wav");
	m_sounds[BOLJER_SOUND] = load_sound("data/boljer.wav");
	m_sounds[KURWA_SOUND] = load_sound("data/kurwa.wav");
	m_sounds[RYSZARD_SOUND] = load_sound("data/ryszard.wav");
	m_sounds[JUMP_SOUND] = load_sound("data/jump.wav");


	m_theme = Mix_LoadMUS("data/zloto.wav");

	// printf("loading background\n");
	this->m_backgroud = Texture::load_texture(this->m_renderer, "data/background.png");

	// printf("loading objects\n");
	auto txt = m_textures[MISKA].clone();
	auto miska = std::make_unique<GameObject>(this->m_window_width/6, this->m_window_height/2,
		txt);
	miska->set_size({ 75, 75 });
	auto collision = miska->get_collision_size();
	miska->set_collision_size(collision * MISKA_HITBOX_SCALE);

	m_game_objects.emplace(MISKA, std::move(miska));

	auto button_font = Font("data/Flappybird_font/FlappyBirdRegular-9Pq0.ttf", 50);

	auto play_button = std::make_unique<Button>((float)m_window_width / 2, ((float)m_window_height / 5) * 4,
		m_window_width / 5, m_window_height / 10, 
		m_textures[BUTTON_TEXTURE].clone(),
		"Graj"s, 
		button_font, 
		m_renderer);
	play_button->set_font_color({ 0, 0, 0 }, m_renderer);

	auto play_pos = play_button->get_pos();
	play_pos.x -= play_button->get_size().x / 2;
	play_button->set_pos(play_pos); 
	m_game_objects.insert({ PLAY_BUTTON, std::move(play_button) });

	auto menu_button = std::make_unique<Button>((float)m_window_width / 2, ((float)m_window_height / 5) * 4,
		m_window_width / 5, m_window_height / 10, 
		m_textures[BUTTON_TEXTURE].clone(),
		"Menu"s, 
		button_font, 
		m_renderer);
	menu_button->set_font_color({ 0, 0, 0 }, m_renderer);
	menu_button->set_pos(play_pos);
	m_game_objects.insert({ MENU_BUTTON, std::move(menu_button) });

	auto ground_one = GameObject(0, m_window_height - (m_window_height / 18), 
		m_textures[GROUND_TEXTURE].clone());
	auto size = ground_one.get_size();
	ground_one.set_size({ (float)m_window_width, size.y });
	m_ground.emplace_back(std::move(ground_one));

	auto ground_two = GameObject(m_window_width, m_window_height - (m_window_height / 18),
		m_textures[GROUND_TEXTURE].clone());
	ground_two.set_size({ (float)m_window_width, size.y });

	m_ground.emplace_back(std::move(ground_two));
	

	auto spacebar_message = std::make_unique<GameObject>(m_window_width / 2, m_window_height / 2,
		(m_window_width / 10) * 3, (m_window_height / 10) * 2, 
		m_textures[SPACEBAR_TEXTURE].clone());
	auto pos = spacebar_message->get_pos();
	size = spacebar_message->get_size();
	spacebar_message->set_pos({ pos.x - size.x / 2, pos.y - size.y / 2 });
	m_game_objects.emplace(SPACEBAR_MESSAGE, std::move(spacebar_message));

	auto score = std::make_unique<TextBox>(m_window_width / 2, m_window_height / 10,
		m_window_width / 12, m_window_height / 10,
		button_font, 
		"1"s, 
		m_renderer);
	pos = score->get_pos();
	size = score->get_size();
	pos.x -= size.x / 2;
	score->set_pos(pos);
	score->set_background_color({ 0xFF, 0, 0, 0xFF });
	score->set_font_color({ 0, 0, 0, 0xFF });
	score->set_text("0", m_renderer);
	m_game_objects.emplace(SCORE, std::move(score));


	auto authorship = std::make_unique<TextBox>(0, m_window_height,
		m_window_width / 5, m_window_height / 30,
		button_font,
		"Made by Adam Papieros"s,
		m_renderer);
	pos = authorship->get_pos();
	size = authorship->get_size();
	pos.x += size.x / 2;
	pos.y -= size.y;
	authorship->set_pos(pos);
	authorship->set_font_color({ 0, 0, 0, 0xFF });
	m_game_objects.emplace(AUTHOR, std::move(authorship));
}

void Game::run() {
	Uint64 last_frame = 0;
	while (!this->m_quit) {
		Uint64 current_frame = SDL_GetTicks64();
		this->m_delta_time = current_frame - last_frame;
		if (this->m_delta_time > 1000 / this->m_fps) {
			last_frame = current_frame;
			auto state = update();
			if (state.has_value()) m_game_state = state.value();
			draw();
		}
	}
}

void Game::draw() {

	SDL_RenderClear(this->m_renderer);

	SDL_RenderCopy(this->m_renderer, m_backgroud.value().get_texture(), NULL, NULL);

	for (auto& item : m_ground) {
		item.draw(m_renderer);
	}

	if (m_game_state == Game::State::Playing || m_game_state == Game::State::Death) {
		m_game_objects[MISKA]->draw(m_renderer);
		for (auto& obstacle : m_obstacles) {
			obstacle.draw(m_renderer, { (float)m_window_width, (float)m_window_height });
		}
		m_game_objects[SCORE]->draw(m_renderer);
	}
	if (m_game_state == Game::State::Menu) {
		m_game_objects[MISKA]->draw(m_renderer);
		m_game_objects[PLAY_BUTTON]->draw(m_renderer);
	}
	if (m_game_state == Game::State::Readyup) {
		m_game_objects[MISKA]->draw(m_renderer);
		m_game_objects[SPACEBAR_MESSAGE]->draw(m_renderer);
	}
	if (m_game_state == Game::State::Death) {
		m_game_objects[MENU_BUTTON]->draw(m_renderer);
	}
	m_game_objects[AUTHOR]->draw(m_renderer);
	SDL_RenderPresent(this->m_renderer);
}

std::optional<Game::State> Game::update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT: {
			this->m_quit = true;
		} break;
		case SDL_KEYDOWN: {
			auto state = keyboard_input(KeyEventData(KeyEventData::Kind::KeyDown, event.key.keysym));
			if (state.has_value()) {
				return state.value();
			}
		} break;
		case SDL_KEYUP: {
			auto state = keyboard_input(KeyEventData(KeyEventData::Kind::KeyUp, event.key.keysym));
			if (state.has_value()) {
				return state.value();
			}
		} break;
		case SDL_MOUSEBUTTONUP: {
			auto state = mouse_input(MouseEventData(MouseEventData::Kind::ButtonUp));
			if (state.has_value()) {
				return state.value();
			}
		} break;
		}
	}

	if (m_game_state != Game::State::Death) {
		auto& front = m_ground.front();
		if (front.get_pos().x + front.get_size().x <= 0) {
			///printf("x: %f  width: %f\n", front.get_pos().x, front.get_size().x);
			auto ground_two = GameObject(m_window_width, m_window_height - (m_window_height / 18),
				m_textures[GROUND_TEXTURE].clone());
			auto size = front.get_size();
			ground_two.set_size(size);

			m_ground.erase(m_ground.begin());
			m_ground.push_back(std::move(ground_two));
		}

		for (auto& g : m_ground) {
	;		auto pos = g.get_pos();
			pos.x += OBSTACLE_VEL;
			g.set_pos(pos);
		}
	}

	//state based stuff
	switch (m_game_state) {
	case Game::State::Menu: {
		auto& miska = m_game_objects.at(MISKA);
		auto miska_pos = miska->get_pos();
		miska_pos.y += sinf(m_sin);
		m_sin += .1;
		miska->set_pos(std::move(miska_pos));

		if (!Mix_PlayingMusic()) {
			Mix_HaltMusic();
			Mix_PlayMusic(m_theme, 0);
		}

	} break;
	case Game::State::Playing: {
		auto& miska = m_game_objects.at(MISKA);
		m_miska_vel += { 0, MISKA_FALL_ACC };
		miska->set_pos(miska->get_pos() + m_miska_vel);

		if (miska->get_pos().y > m_window_height) {
			m_miska_vel = { 0, 0 };
			Mix_PlayChannel(-1, m_sounds[DEBIL_SOUND], 0);
			return Game::State::Death;
		}

		auto add_obstacle = [&]() {
			m_dist = std::uniform_real_distribution<float>(.5, .75);
			auto offset = std::rand() % 2 == 0 ? m_dist(m_rng) : -m_dist(m_rng);
			m_dist = std::uniform_real_distribution<float>(miska->get_size().x * 2, 250);
			auto gap = m_dist(m_rng);
			// printf("offset: %f\ngap: %f\n", offset, gap);

			auto tooth_t = m_textures[TOOTH_TEXTURE];

			this->m_obstacles.push_back({ this->m_renderer,
					(unsigned int)gap,
					(int)((this->m_window_width / 10) * OBSTACLE_WIDTH_SCALE),
					{ (float)this->m_window_width, 0 },
					offset,
					tooth_t});
		};

		if (m_obstacles.size() < 6) {
			if (m_obstacles.empty()) {
				add_obstacle();
			}
			else if (m_obstacles.back().get_pos().x + m_obstacles.back().get_width() + (m_window_width / 10) * OBSTACLE_SPACING_SCALE <= m_window_width) {
				add_obstacle();
			}
		}
		for (auto& obstacle : m_obstacles) {
			auto pos = obstacle.get_pos();
			pos += Vector2{ OBSTACLE_VEL, 0 };
			obstacle.set_pos(pos);

			if (obstacle.check_collision(miska->get_collider(), { (float)m_window_width, (float)m_window_height })) {
				m_miska_vel = { 0, 0 };
				Mix_PlayChannel(-1, m_sounds[DEATH_SOUND], 0);
				return Game::State::Death;
			}
		}

		if (!m_obstacles.empty()) {
			auto& front = m_obstacles.front();
			if (front.get_pos().x + front.get_width() / 2 <= miska->get_pos().x && !m_score_flag) {
				m_score++;
				m_score_flag = true;
				auto score = m_game_objects[SCORE].get();
				auto score_ptr = dynamic_cast<TextBox*>(score);
				score_ptr->set_text(std::to_string(m_score), m_renderer);
				if (m_score % 1000 == 0) {
					Mix_PlayChannel(-1, m_sounds[KURWA_SOUND], 0);
				}
				if (m_score % 500 == 0) {
					Mix_PlayChannel(-1, m_sounds[POLICJA_SOUND], 0);
				}
				else if (m_score % 100 == 0) {;
					Mix_PlayChannel(-1, m_sounds[TATA_SOUND], 0);
				}
				else if (m_score % 50 == 0) {
					Mix_PlayChannel(-1, m_sounds[RYSZARD_SOUND], 0);
				}
				else if (m_score % 25 == 0) {
					Mix_PlayChannel(-1, m_sounds[KOLEZANKO_SOUND], 0);
				}
				else if (m_score % 10 == 0) {
					Mix_PlayChannel(-1, m_sounds[OOO_SOUND], 0);
				}
				else {
					Mix_PlayChannel(-1, m_sounds[GAGRI_SOUND], 0);
				}

				// printf("score++\n");
			}
			if (front.get_pos().x + front.get_width() < 0) {
				m_obstacles.erase(m_obstacles.begin());
				m_score_flag = false;
			}
		}
		} break;
	}
	return std::nullopt;
}

std::optional<Game::State> Game::keyboard_input(KeyEventData data) {
	using Kind = KeyEventData::Kind;
	if (data.kind == Kind::KeyUp && data.keysym.sym == SDLK_SPACE && m_game_state == Game::State::Playing) {
		m_miska_vel += {0, MISKA_JUMP};
		Mix_PlayChannel(-1, m_sounds[JUMP_SOUND], 0);
	}
	if (data.kind == Kind::KeyUp && data.keysym.sym == SDLK_SPACE && m_game_state == Game::State::Readyup) {
		Mix_HaltChannel(-1);
		return Game::State::Playing;
	}
	return std::nullopt;
}

std::optional<Game::State> Game::mouse_input(MouseEventData data) {
	using Kind = MouseEventData::Kind;

	if (m_game_state == Game::State::Playing)
		return std::nullopt;

	int mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	auto& play_button = m_game_objects[PLAY_BUTTON];
	auto play_button_rect = play_button->get_rect();
	auto menu_button_rect = m_game_objects[MENU_BUTTON]->get_rect();

	if (m_game_state == Game::State::Menu && data.kind == Kind::ButtonUp &&
		mouse_x >= play_button_rect.x && mouse_x <= play_button_rect.x + play_button_rect.w &&
		mouse_y >= play_button_rect.y && mouse_y <= play_button_rect.y + play_button_rect.h) 
	{
		m_game_objects[MISKA]->set_pos({ (float)this->m_window_width / 6, (float)this->m_window_height / 2 });
		m_sin = .0;
		Mix_HaltMusic();
		Mix_PlayChannel(-1, m_sounds[BOLJER_SOUND], 0);
		return Game::State::Readyup;
	}

	if (m_game_state == Game::State::Death && data.kind == Kind::ButtonUp &&
		mouse_x >= menu_button_rect.x && mouse_x <= menu_button_rect.x + menu_button_rect.w &&
		mouse_y >= menu_button_rect.y && mouse_y <= menu_button_rect.y + menu_button_rect.h)
	{
		m_game_objects[MISKA]->set_pos({ (float)this->m_window_width / 6, (float)this->m_window_height / 2 });
		m_sin = .0;
		m_obstacles.clear();
		m_score = 0;
		m_score_flag = false;
		auto score_ptr = m_game_objects[SCORE].get();
		auto score = dynamic_cast<TextBox*>(score_ptr);
		score->set_text(std::to_string(m_score), m_renderer);
		Mix_HaltChannel(-1);
		return Game::State::Menu;
	}
	return std::nullopt;
}

void Game::close() {
	SDL_DestroyRenderer(this->m_renderer);
	SDL_DestroyWindow(this->m_main_window);
	m_game_objects.clear();
	Mix_FreeMusic(m_theme);
	for (auto& [_, s] : m_sounds) {
		Mix_FreeChunk(s);
	}
	m_textures.clear();
	
}

KeyEventData::KeyEventData(Kind kind, SDL_Keysym sym) :
	kind{kind},
	keysym{sym}
{

}

MouseEventData::MouseEventData(MouseEventData::Kind kind) :
	kind{ kind }
{

}
