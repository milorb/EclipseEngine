#include "Engine.hpp"

#include "Tiled.hpp"

void render_();
void render_ui();
void render_text();
void render_pixels();

void Engine::initialize() {
    
    PackageManager::load_packages();
    
    ComponentDB::init();
    ImageDB::init();
    Physics::init();
    SceneDB::init();
    AudioDB::init();
    FontDB::init();
    Input::init();
    Event::init();
    
    init_game_config();
    
    Map m;
    m.parse("hall");
    m.populate_actors();
}

void Engine::game_loop() {

    initialize();

    while (!quit) {
        input();
        update();
        render();
    }

    return;
}

void Engine::input() {

    SDL_RenderClear(Renderer.__Renderer);

    if (SceneDB::to_load.first) {
        CurScene.switch_scene();
    }

    SDL_Event e;
    while (Helper::SDL_PollEvent498(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
        Input::process_event(e);
    }
}

void Engine::update() {
    
    SceneDB::start();
    SceneDB::update();
    SceneDB::update_L();
    SceneDB::destroy_acs();
    SceneDB::instantiate_acs();
    SceneDB::add_components();
    SceneDB::remove_components();
    
    Input::update_frame_end();

    Event::update_subscriptions();
    
    if (Physics::__World) {
        Physics::__World->Step(1.0f / 60.0f, 8, 3);
    }
}

void Engine::render() {
    
    glm::vec3 cc = Renderer::clear_color;
    SDL_SetRenderDrawColor( 
        Renderer::__Renderer, 
        cc.x, 
        cc.y, 
        cc.z, 255);

    SDL_RenderSetScale( 
        Renderer::__Renderer,
        Renderer::zoom_factor,
        Renderer::zoom_factor);

    render_();

    SDL_RenderSetScale(
        Renderer::__Renderer, 
        1.0f, 1.0f);

    render_ui();
    render_text();
    render_pixels();

    Helper::SDL_RenderPresent498(
        Renderer::__Renderer);
}

// draws to scene space
void render_() {
    std::stable_sort(
        Renderer::scene_q.begin(),
        Renderer::scene_q.end(),
        &DrawSceneSpace::compare_sorting_order);

    for (auto& fn : 
    Renderer::scene_q) (*fn)();
    Renderer::scene_q.clear();
    Renderer::scene_q.resize(0);
}

void render_ui() {
    std::stable_sort(
        Renderer::ui_q.begin(),
        Renderer::ui_q.end(),
        &DrawUISpace::compare_sorting_order);

    for (auto& fn : 
    Renderer::ui_q) (*fn)();
    Renderer::ui_q.clear();
    Renderer::ui_q.resize(0);
}

void render_text() {
    for (auto& fn : 
    Renderer::text_q) (*fn)();
    Renderer::text_q.clear();
    Renderer::text_q.resize(0);
}

void render_pixels() {
    SDL_SetRenderDrawBlendMode(
        Renderer::__Renderer, 
        SDL_BLENDMODE_BLEND);
    
    for (auto& px : Renderer::px_q) {
        SDL_SetRenderDrawColor(
            Renderer::__Renderer,
            px->r, 
            px->g, 
            px->b, 
            px->a);
        SDL_RenderDrawPoint(
            Renderer::__Renderer, 
            px->x, 
            px->y);
        delete px;
    }

    Renderer::px_q.clear();
    Renderer::px_q.resize(0);

    SDL_SetRenderDrawColor(
        Renderer::__Renderer,
        Renderer::clear_color.x,
        Renderer::clear_color.y,
        Renderer::clear_color.z,
        255);

    SDL_SetRenderDrawBlendMode(
        Renderer::__Renderer,
        SDL_BLENDMODE_NONE);
}

void validate_resources_dir() {
    // ensure all necessary files/paths exist
    if (!std::filesystem::exists("resources")) {
        std::cout << "error: resources/ missing";
        std::exit(0);
    }
    if (!std::filesystem::exists("resources/game.config")) {
        std::cout << "error: resources/game.config missing";
        std::exit(0);
    }
}

void Engine::init_game_config() {

    validate_resources_dir();
    // now load game specific stuff from the files
    Utilities::read_json_file(
        "resources/game.config", 
        game_config);
    
    if (game_config.HasMember("initial_scene")) {
        if (game_config.HasMember("game_title")) {
            game_title = game_config["game_title"].GetString();
        }
        Renderer.init(game_title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED);

        //make sure to initialize the componentDB in order to have LuaRef
        //now done in Engine::init()
        std::string scene_name = game_config["initial_scene"].GetString();
        CurScene.load_scene(scene_name);
    }
    else {
        std::cout << "error: initial_scene unspecified";
        std::exit(0);
    }
}
