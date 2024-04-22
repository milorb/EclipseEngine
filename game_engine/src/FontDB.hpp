#ifndef FontDB_hpp
#define FontDB_hpp

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <stdio.h>
#include <vector>
#include <string>

#include "lua.hpp"
#include "LuaBridge.h"
#include "rapidjson/document.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "SDL2_image/SDL_image.h"
#include "ComponentDB.hpp"
#include "Renderer.hpp"
#include "glm/glm.hpp"

class FontDB {
public:
    static void init();
    static void load_font(std::string font_name, int font_size);
    static SDL_Texture* string_to_texture(std::string& text, int r, int g, int b, int a,
        std::string& font_name, int font_size, SDL_Renderer* renderer);

    static void Draw(std::string content, float x, float y, 
        std::string font_name, float font_size, 
        float r, float g, float b, float a);

    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;  
};

#endif
