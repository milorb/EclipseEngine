#include "FontDB.hpp"

void FontDB::init() {
    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Text")
        .addFunction("Draw",
            &FontDB::Draw)
        .endNamespace();
}

void FontDB::load_font(std::string font_name, int font_size) {
    std::string filepath = "resources/fonts/" + font_name + ".ttf";
    if (!std::filesystem::exists(filepath)) {
            std::cout << "error: font " << font_name << " missing";
            std::exit(0);
    }
    
    TTF_Init();
    
    TTF_Font* font = TTF_OpenFont(filepath.c_str(), font_size);
    fonts[font_name][font_size] = font;

    if (!font) {
        std::cout << "error: failed to initialize texture " << SDL_GetError();
        std::exit(0);
    }
}

SDL_Texture* FontDB::string_to_texture(std::string& text, int r, int g, int b, int a,
    std::string& font_name, int font_size, SDL_Renderer* renderer) {

    TTF_Init();

    TTF_Font* font = fonts[font_name][font_size];

    SDL_Color color = {Uint8(r), Uint8(g), Uint8(b), Uint8(a) };

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);

    if (!surface) {
        std::cout << "error: failed to create surface " << TTF_GetError();
        std::exit(0);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cout << "error: failed to create texture " << SDL_GetError();
        std::exit(0);
    }
    SDL_FreeSurface(surface);
    return texture;
}

void FontDB::Draw(std::string content, float _x, float _y,
    std::string font_name, float _font_size,
    float _r, float _g, float _b, float _a) {

    int font_size = (int)_font_size;
    int r = (int)_r;
    int g = (int)_g;
    int b = (int)_b;
    int a = (int)_a;

    if (fonts.find(font_name) == fonts.end()) {
        load_font(font_name, font_size);
    }
    else if (fonts[font_name].find(font_size) == fonts[font_name].end()) {
        load_font(font_name, font_size);
    }

    SDL_Texture* text_to_render = string_to_texture(content, 
        r, g, b, a, font_name, font_size, Renderer::__Renderer);
    
    std::shared_ptr<Fn_DrawText> fn = std::make_shared<Fn_DrawText>(text_to_render, _x, _y);

    Renderer::text_q.push_back(fn);
}
