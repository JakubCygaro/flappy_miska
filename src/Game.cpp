#include "Game.h"
#include "Button.h"
#include "TextBox.h"
#include "Vector2.h"
#include <SDL2/SDL_timer.h>
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

EM_JS(void, register_game, (), {
    registerGameImpl();
});

EM_JS(void, register_game_end, (), {
    registerGameEndImpl();
});

EM_JS(void, upload_score, (int score), {
    uploadScoreImpl(score);
});

EM_JS(int, set_or_get_personal_best, (int score), {
    return setOrGetPersonalBestImpl(score);
});

EM_JS(void, heartbeat, (), {
    heartbeatImpl();
})

#endif

using namespace std::literals::string_literals;

static Mix_Chunk* load_sound(const char* path)
{
    auto sound = Mix_LoadWAV(path);
    if (sound == NULL) {
        throw std::runtime_error(std::format("could not load `{}`. {}\n", path, Mix_GetError()));
    }
    return sound;
}

Game::Game(int w, int h, float fps)
    : m_window_height { h }
    , m_window_width { w }
    , m_renderer { NULL }
    , m_main_window { NULL }
    , m_game_objects { std::map<std::string, std::unique_ptr<GameObject>>() }
    , m_quit { false }
    , m_delta_time { 0 }
    , m_fps { fps }
    , m_miska_vel { Vector2 { 0, 1 } }
    , m_obstacles { std::vector<Teeth>() }
    , m_game_state { State::Menu }
    , m_sin { 0.0 }
    , m_ground { std::vector<GameObject>() }
    , m_score { 0 }
    , m_score_flag { false }
    , m_rand_dev { std::random_device() }
    , m_rng { std::mt19937(m_rand_dev()) }
    , m_dist {}
    , m_textures { std::map<std::string, Texture>() }
    , m_theme { NULL }
{
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    m_rng.seed(seed);
}

void Game::initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        throw std::runtime_error(std::format("could not initialize {}\n", SDL_GetError()));
    }

    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("Warning: linear texture filtering not enabled\n");
    }

    SDL_DisplayMode display;
    if (SDL_GetDesktopDisplayMode(0, &display)) {
        throw std::runtime_error("Could not determine the display size\n");
    }
    if (display.h < m_window_height)
        m_window_height = 0.80 * display.h;

    if (display.w < m_window_width)
        m_window_width = 0.67 * display.w;

    this->m_main_window = SDL_CreateWindow("Flappy Miska",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        this->m_window_width, this->m_window_height,
        SDL_WINDOW_SHOWN);

    if (this->m_main_window == NULL) {
        throw std::runtime_error(std::format("could not create main window {}\n", SDL_GetError()));
    }
    this->m_renderer = SDL_CreateRenderer(this->m_main_window, -1, SDL_RENDERER_ACCELERATED);

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

void Game::initialize_objects()
{

    m_textures.emplace(MISKA, Texture::load_texture(m_renderer, "data/miska.png"));
    m_textures.emplace(BUTTON_TEXTURE, Texture::load_texture(m_renderer, "data/button.png"));
    m_textures.emplace(GROUND_TEXTURE, Texture::load_texture(m_renderer, "data/ground.png"));
    m_textures.emplace(SPACEBAR_TEXTURE, Texture::load_texture(m_renderer, "data/space.png"));
    m_textures.emplace(TOOTH_TEXTURE, Texture::load_texture(m_renderer, "data/zab.png"));

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

    this->m_backgroud = Texture::load_texture(this->m_renderer, "data/background.png");

    auto txt = m_textures[MISKA].clone();
    auto miska = std::make_unique<GameObject>(this->m_window_width / 6, this->m_window_height / 2,
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
    play_button->set_font_color({ 0, 0, 0, 255 }, m_renderer);

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
    menu_button->set_font_color({ 0, 0, 0, 255 }, m_renderer);
    menu_button->set_pos(play_pos);
    m_game_objects.insert({ MENU_BUTTON, std::move(menu_button) });

    auto ground_one = GameObject(0, m_window_height - (m_window_height / 18.),
        m_textures[GROUND_TEXTURE].clone());
    auto size = ground_one.get_size();
    ground_one.set_size({ (float)m_window_width, size.y });
    m_ground.emplace_back(std::move(ground_one));

    auto ground_two = GameObject(m_window_width, m_window_height - (m_window_height / 18.),
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
    auto score_size = score->get_size();
    pos.x -= score_size.x / 2;
    score->set_pos(pos);
    score->set_background_color({ 0xFF, 0, 0, 0xFF });
    score->set_font_color({ 0, 0, 0, 0xFF });
    score->set_text("0", m_renderer);
    m_game_objects.emplace(SCORE, std::move(score));

#ifndef __EMSCRIPTEN__
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
#else
    auto best_text = std::make_unique<TextBox>(m_window_width / 2, m_window_height / 10,
        m_window_width / 10, m_window_height / 10,
        button_font,
        "0"s,
        m_renderer);
    auto best_text_pos = best_text->get_pos();
    auto best_text_size = best_text->get_size();
    best_text_pos.x -= best_text_size.x / 2;
    best_text_pos.y += score_size.y * 1.5;
    best_text->set_pos(best_text_pos);
    best_text->set_background_color({ 0xFF, 0, 0, 0xFF });
    best_text->set_font_color({ 0, 0, 0, 0xFF });
    best_text->set_text("BEST", m_renderer);
    m_game_objects.emplace(BEST_TEXT, std::move(best_text));

    auto personal_top_score = std::make_unique<TextBox>(m_window_width / 2, m_window_height / 10,
        m_window_width / 10, m_window_height / 10,
        button_font,
        "-1"s,
        m_renderer);
    pos = personal_top_score->get_pos();
    size = personal_top_score->get_size();
    pos.x -= size.x / 2;
    pos.y = best_text_pos.y;
    pos.y += best_text_size.y * 1.1;
    personal_top_score->set_pos(pos);
    personal_top_score->set_background_color({ 0xFF, 0, 0, 0xFF });
    personal_top_score->set_font_color({ 0, 0, 0, 0xFF });
    personal_top_score->set_text("-1", m_renderer);
    m_game_objects.emplace(PERSONAL_TOP_SCORE, std::move(personal_top_score));

    auto menu_btn_pos = m_game_objects[MENU_BUTTON]->get_pos();
    auto upload_score_btn = std::make_unique<Button>((float)m_window_width / 2, ((float)m_window_height / 5) * 4,
        m_window_width / 3, m_window_height / 10,
        m_textures[BUTTON_TEXTURE].clone(),
        "Upload Score"s,
        button_font,
        m_renderer);
    upload_score_btn->set_font_color({ 0, 0, 0, 255 }, m_renderer);
    size = upload_score_btn->get_size();
    auto upload_score_btn_pos = upload_score_btn->get_pos();
    upload_score_btn_pos.y = menu_btn_pos.y - size.y * 1.15;
    upload_score_btn_pos.x -= size.x / 2.;
    upload_score_btn->set_pos(upload_score_btn_pos);
    m_game_objects.insert({ UPLOAD_SCORE_BUTTON, std::move(upload_score_btn) });
#endif
}

void Game::run()
{
    Uint64 last_frame = 0;
    while (!this->m_quit) {
        Uint64 current_frame = SDL_GetTicks64();
        this->m_delta_time = current_frame - last_frame;
        if (this->m_delta_time > 1000 / this->m_fps) {
            last_frame = current_frame;
            auto state = update();
            if (state.has_value())
                m_game_state = state.value();
            draw();
        }
    }
}

void Game::draw()
{

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
#ifdef __EMSCRIPTEN__
        m_game_objects[UPLOAD_SCORE_BUTTON]->draw(m_renderer);
        m_game_objects[BEST_TEXT]->draw(m_renderer);
        m_game_objects[PERSONAL_TOP_SCORE]->draw(m_renderer);
#endif
    }
#ifndef __EMSCRIPTEN__
    m_game_objects[AUTHOR]->draw(m_renderer);
#endif
    SDL_RenderPresent(this->m_renderer);
}

std::optional<Game::State> Game::update()
{
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
            auto ground_two = GameObject(m_window_width, m_window_height - (m_window_height / 18.),
                m_textures[GROUND_TEXTURE].clone());
            auto size = front.get_size();
            ground_two.set_size(size);

            m_ground.erase(m_ground.begin());
            m_ground.push_back(std::move(ground_two));
        }

        for (auto& g : m_ground) {
            ;
            auto pos = g.get_pos();
            pos.x += OBSTACLE_VEL;
            g.set_pos(pos);
        }
    }

    // state based stuff
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
#ifdef __EMSCRIPTEN__
            register_game_end();
            int best = set_or_get_personal_best(this->m_score);
            dynamic_cast<TextBox*>(m_game_objects[PERSONAL_TOP_SCORE].get())->set_text(std::to_string(best), m_renderer);
#endif
            return Game::State::Death;
        }

        auto add_obstacle = [&]() {
            m_dist = std::uniform_real_distribution<float>(.5, .75);
            auto offset = std::rand() % 2 == 0 ? m_dist(m_rng) : -m_dist(m_rng);
            m_dist = std::uniform_real_distribution<float>(miska->get_size().x * 2, 250);
            auto gap = m_dist(m_rng);

            auto tooth_t = m_textures[TOOTH_TEXTURE];

            this->m_obstacles.push_back({ this->m_renderer,
                (unsigned int)gap,
                (int)((this->m_window_width / 10.) * OBSTACLE_WIDTH_SCALE),
                { (float)this->m_window_width, 0 },
                offset,
                tooth_t });
        };

        if (m_obstacles.size() < 6) {
            if (m_obstacles.empty()) {
                add_obstacle();
            } else if (m_obstacles.back().get_pos().x + m_obstacles.back().get_width() + (m_window_width / 10.) * OBSTACLE_SPACING_SCALE <= m_window_width) {
                add_obstacle();
            }
        }
        for (auto& obstacle : m_obstacles) {
            auto pos = obstacle.get_pos();
            pos += Vector2 { OBSTACLE_VEL, 0 };
            obstacle.set_pos(pos);

            if (obstacle.check_collision(miska->get_collider(), { (float)m_window_width, (float)m_window_height })) {
                m_miska_vel = { 0, 0 };
                Mix_PlayChannel(-1, m_sounds[DEATH_SOUND], 0);
#ifdef __EMSCRIPTEN__
                register_game_end();
                int best = set_or_get_personal_best(this->m_score);
                dynamic_cast<TextBox*>(m_game_objects[PERSONAL_TOP_SCORE].get())->set_text(std::to_string(best), m_renderer);
#endif
                return Game::State::Death;
            }
        }

        if (!m_obstacles.empty()) {
            auto& front = m_obstacles.front();
            if (front.get_pos().x + front.get_width() / 2. <= miska->get_pos().x && !m_score_flag) {
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
                } else if (m_score % 100 == 0) {
                    ;
                    Mix_PlayChannel(-1, m_sounds[TATA_SOUND], 0);
                } else if (m_score % 50 == 0) {
                    Mix_PlayChannel(-1, m_sounds[RYSZARD_SOUND], 0);
                } else if (m_score % 25 == 0) {
                    Mix_PlayChannel(-1, m_sounds[KOLEZANKO_SOUND], 0);
                } else if (m_score % 10 == 0) {
                    Mix_PlayChannel(-1, m_sounds[OOO_SOUND], 0);
                } else {
                    Mix_PlayChannel(-1, m_sounds[GAGRI_SOUND], 0);
                }
            }
            if (front.get_pos().x + front.get_width() < 0) {
                m_obstacles.erase(m_obstacles.begin());
                m_score_flag = false;
            }
#ifdef __EMSCRIPTEN__
            static Uint64 last_heartbeat = 0;
            Uint64 current_frame = SDL_GetTicks64();
            if (current_frame - last_heartbeat >= 1000 * 60 * 3) {
                last_heartbeat = current_frame;
                heartbeat();
            }
#endif
        }
    } break;
    default:
        break;
    }
    return std::nullopt;
}

std::optional<Game::State> Game::keyboard_input(KeyEventData data)
{
    using Kind = KeyEventData::Kind;
    if (data.kind == Kind::KeyUp && data.keysym.sym == SDLK_SPACE && m_game_state == Game::State::Playing) {
        m_miska_vel += { 0, MISKA_JUMP };
        Mix_PlayChannel(-1, m_sounds[JUMP_SOUND], 0);
    }
    if (data.kind == Kind::KeyUp && data.keysym.sym == SDLK_SPACE && m_game_state == Game::State::Readyup) {
        Mix_HaltChannel(-1);
#ifdef __EMSCRIPTEN__
        register_game();
#endif
        return Game::State::Playing;
    }
    return std::nullopt;
}

std::optional<Game::State> Game::mouse_input(MouseEventData data)
{
    using Kind = MouseEventData::Kind;

    if (m_game_state == Game::State::Playing)
        return std::nullopt;

    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    Vector2 mouse = { .x = static_cast<float>(mouse_x), .y = static_cast<float>(mouse_y) };
    auto& play_button = m_game_objects[PLAY_BUTTON];
    auto& menu_button = m_game_objects[MENU_BUTTON];

    if (m_game_state == Game::State::Menu && data.kind == Kind::ButtonUp && GameObject::point_collision(play_button.get(), mouse)) {
        m_game_objects[MISKA]->set_pos({ (float)this->m_window_width / 6, (float)this->m_window_height / 2 });
        m_sin = .0;
        Mix_HaltMusic();
        Mix_PlayChannel(-1, m_sounds[BOLJER_SOUND], 0);
        return Game::State::Readyup;
    }

    if (m_game_state == Game::State::Death && data.kind == Kind::ButtonUp && GameObject::point_collision(menu_button.get(), mouse)) {
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
#ifdef __EMSCRIPTEN__
    auto& upload_button = m_game_objects[UPLOAD_SCORE_BUTTON];
    if (m_game_state == Game::State::Death && data.kind == Kind::ButtonUp && GameObject::point_collision(upload_button.get(), mouse)) {
        upload_score(this->m_score);
    }
#endif
    return std::nullopt;
}

void Game::close()
{
    SDL_DestroyRenderer(this->m_renderer);
    SDL_DestroyWindow(this->m_main_window);
    m_game_objects.clear();
    Mix_FreeMusic(m_theme);
    for (auto& [_, s] : m_sounds) {
        Mix_FreeChunk(s);
    }
    m_textures.clear();
}

KeyEventData::KeyEventData(Kind kind, SDL_Keysym sym)
    : kind { kind }
    , keysym { sym }
{
}

MouseEventData::MouseEventData(MouseEventData::Kind kind)
    : kind { kind }
{
}
