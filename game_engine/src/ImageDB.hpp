#ifndef ImageDB_hpp
#define ImageDB_hpp

#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <functional>
#include <filesystem>
#include <unordered_map>

#include "rapidjson/document.h"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"

#include "Helper.h"
#include "Renderer.hpp"
#include "Actor.hpp"



class ImageDB {
public:
    static void init();

    static void load_image(std::string filename);

    static void DrawUI(std::string img_name, float x, float y);

    static void DrawUIEx(std::string img_name, float x, float y, 
        float r, float g, float b, float a, float sorting_order);

    static void Draw(std::string img_name, float x, float y);

    static void DrawEx(std::string img_name, float x, float y, float rotation,
        float scale_x, float scale_y, float pivot_x, float pivot_y,
        float r, float g, float b, float a, float sorting_order);

    static void DrawPixel(float x, float y, float r, float g, float b, float a);

    static inline std::unordered_map<std::string, SDL_Texture*> images;
};

#endif /* ImageDB_hpp */
