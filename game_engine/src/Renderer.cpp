#include "Renderer.hpp"

void Renderer::init_rendering_config() {
    
    if (std::filesystem::exists("resources/rendering.config")) {
        
        Utilities::read_json_file("resources/rendering.config", rendering_config);
        
        if (rendering_config.HasMember("x_resolution")) {
            resolution.x = rendering_config["x_resolution"].GetInt();
        }
        if (rendering_config.HasMember("y_resolution")) {
            resolution.y = rendering_config["y_resolution"].GetInt();
        }
        if (rendering_config.HasMember("clear_color_r")) {
            clear_color.x = rendering_config["clear_color_r"].GetInt();
        }
        if (rendering_config.HasMember("clear_color_g")) {
            clear_color.y = rendering_config["clear_color_g"].GetInt();
        }
        if (rendering_config.HasMember("clear_color_b")) {
            clear_color.z = rendering_config["clear_color_b"].GetInt();
        }
        if (rendering_config.HasMember("zoom_factor")) {
            zoom_factor = rendering_config["zoom_factor"].GetFloat();
        }
    }
}

void Renderer::CamSetPosition(float x, float y) {
    camera_pos = { x, y };
}

float Renderer::CamGetPositionX() {
    return camera_pos.x;
}

float Renderer::CamGetPositionY() {
    return camera_pos.y;
}

void Renderer::CamSetZoom(float zoom) {
    zoom_factor = zoom;
}

float Renderer::CamGetZoom() {
    return zoom_factor;
}

void Renderer::init(const char *title, int x, int y) {
    init_rendering_config();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL initialization failed, Error: " << SDL_GetError();
        exit(0);
    }

    camera_pos = { 0, 0 };
    
    SDL_Window* window = Helper::SDL_CreateWindow498(
        title, 
        x, y, 
        resolution.x, resolution.y, 
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::cout << "SDL window creation failed, Error: " << SDL_GetError();
        exit(0);
    }
    
    SDL_Renderer* renderer = Helper::SDL_CreateRenderer498(
        window, 
        -1, 
        SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    if (!window) {
        std::cout << "SDL renderer creation failed, Error: " << SDL_GetError();
        exit(0);
    }
    
    __Window = window;
    __Renderer = renderer;
    
    SDL_SetRenderDrawColor(renderer, 
        clear_color.x, 
        clear_color.y, 
        clear_color.z, 
        255);

    SDL_RenderClear(renderer);
}

void Renderer::render_text(int x, int y, SDL_Texture * text) {
    int w, h;
    SDL_QueryTexture(text, NULL, NULL, &w, &h);
    SDL_Rect text_loc = {x,  y, w, h};
    SDL_RenderCopy(__Renderer,
                   text,
                   NULL,
                   &text_loc);
}


void Fn_Draw::operator()() {

    float zoom = Renderer::zoom_factor;
    int w, h;

    SDL_QueryTexture(image, NULL, NULL, &w, &h);
    SDL_Point pivot = { static_cast<int>(w * 0.5f), static_cast<int>(h * 0.5f) };
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    float shift_x = (x - Renderer::camera_pos.x) * 100;
    float zoom_x = (Renderer::resolution.x / 2) / zoom;
    int render_x = static_cast<int>(
        shift_x + zoom_x - pivot.x);

    float shift_y = (y - Renderer::camera_pos.y) * 100;
    float zoom_y = (Renderer::resolution.y / 2) / zoom;
    int render_y = static_cast<int>(
        shift_y + zoom_y - pivot.y);

    SDL_Rect dstrct = {
        render_x,
        render_y,
        w,
        h
    };

    Helper::SDL_RenderCopyEx498(0, "",
        Renderer::__Renderer,
        image,
        NULL,
        &dstrct,
        0,
        &pivot,
        flip);
}

void Fn_DrawEx::operator()() {

    float zoom = Renderer::zoom_factor;
    int w, h;

    SDL_QueryTexture(image, NULL, NULL, &w, &h);

    int scaled_w = std::abs(scale_x) * w;
    int scaled_h = std::abs(scale_y) * h;

    SDL_Point pivot = { static_cast<int>(scaled_w * pivot_x), 
        static_cast<int>(scaled_h * pivot_y) };

    SDL_RendererFlip flipH = SDL_FLIP_NONE;
    SDL_RendererFlip flipV = SDL_FLIP_NONE;
    if (scale_x < 0) flipH = SDL_FLIP_HORIZONTAL;
    if (scale_y < 0) flipV = SDL_FLIP_VERTICAL;

    float shift_x = (x - Renderer::camera_pos.x);
    float zoom_x = (Renderer::resolution.x * 0.5f) 
        * (1.0f / zoom);
    int render_x = static_cast<int>(
        shift_x + zoom_x -  pivot.x);

    float shift_y = (y - Renderer::camera_pos.y);
    float zoom_y = (Renderer::resolution.y * 0.5f) 
        * (1.0f / zoom);
    int render_y = static_cast<int>(
        shift_y + zoom_y - pivot.y);

    SDL_Rect dstrct = {
        render_x,
        render_y,
        scaled_w,
        scaled_h
    };

    SDL_SetTextureColorMod(image, r, g, b);
    SDL_SetTextureAlphaMod(image, a);

    Helper::SDL_RenderCopyEx498(0, "",
        Renderer::__Renderer,
        image,
        NULL,
        &dstrct,
        rotation,
        &pivot,
        SDL_RendererFlip(flipV | flipH));

    SDL_SetTextureColorMod(image, 255, 255, 255);
    SDL_SetTextureAlphaMod(image, 255);
}

void Fn_DrawUI::operator()() {

    int w, h;
    SDL_QueryTexture(image, NULL, NULL, &w, &h);
    SDL_Rect dstrct = { x, y, w, h };
    SDL_Point center = { w / 2, h / 2 };

    Helper::SDL_RenderCopyEx498(0, "",
        Renderer::__Renderer,
        image,
        NULL,
        &dstrct,
        0,
        &center,
        SDL_FLIP_NONE);
}

void Fn_DrawUIEx::operator()() {

    int w, h;
    SDL_QueryTexture(image, NULL, NULL, &w, &h);
    SDL_Rect dstrct = { x, y, w, h };
    SDL_Point center = { w / 2, h / 2 };
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    SDL_SetTextureColorMod(image, r, g, b);
    SDL_SetTextureAlphaMod(image, a);

    Helper::SDL_RenderCopyEx498(0, "",
        Renderer::__Renderer,
        image,
        NULL,
        &dstrct,
        0,
        &center,
        flip);

    SDL_SetTextureColorMod(image, 255, 255, 255);
    SDL_SetTextureAlphaMod(image, 255);
}

/*
void Fn_DrawPixel::operator()() {

    SDL_SetRenderDrawBlendMode(Renderer::__Renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(Renderer::__Renderer,
        r, g, b, a);
    SDL_RenderDrawPoint(Renderer::__Renderer, x, y);

    SDL_SetRenderDrawColor(Renderer::__Renderer,
        Renderer::clear_color.x,
        Renderer::clear_color.y,
        Renderer::clear_color.z,
        255);

    SDL_SetRenderDrawBlendMode(Renderer::__Renderer, SDL_BLENDMODE_NONE);
}
*/

void Fn_DrawText::operator()() {
    int w, h;
    SDL_QueryTexture(text_to_render, NULL, NULL, &w, &h);
    SDL_Rect text_loc = { x,  y, w, h };
    SDL_Point center = { w / 2, h / 2 };

    SDL_RendererFlip flip = SDL_FLIP_NONE;

    Helper::SDL_RenderCopyEx498(0, "",
        Renderer::__Renderer,
        text_to_render,
        NULL,
        &text_loc,
        0,
        &center,
        flip);
}
