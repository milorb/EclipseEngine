#ifndef Renderer_h
#define Renderer_h

#include "lua.hpp"
#include "LuaBridge.h"
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "Helper.h"
#include "FontDB.hpp"
#include "EngineUtils.h"

class DrawSceneSpace;
class DrawUISpace;
class Fn_DrawText;
class Fn_DrawPixel;

class Renderer {
public:
    void init(const char* title, int x, int y);
    void init_rendering_config();
    void render_text(int x, int y, SDL_Texture* text);

    static void CamSetPosition(float x, float y);
    static void CamSetZoom(float zoom);
    static float CamGetPositionX();
    static float CamGetPositionY();
    static float CamGetZoom();

    rapidjson::Document rendering_config;

    static inline SDL_Renderer* __Renderer;
    static inline SDL_Window* __Window;

    static inline glm::ivec3 clear_color = {255, 255, 255};
    static inline glm::vec2 resolution = { 640, 360 };
    static inline glm::vec2 camera_pos;

    static inline float zoom_factor = 1.0f;

    static inline std::vector<std::shared_ptr<DrawSceneSpace>> scene_q;
    static inline std::vector<Fn_DrawPixel*> px_q;
    static inline std::vector<std::shared_ptr<Fn_DrawText>> text_q;
    static inline std::vector<std::shared_ptr<DrawUISpace>> ui_q;
};


//* Base class for drawing sprites to screen space
class DrawSceneSpace {
public:
    DrawSceneSpace(std::string _img_name, SDL_Texture* _image, 
        float _x, float _y) 
        :
        img_name(_img_name),
        image(_image),
        x(_x),
        y(_y) {}

    virtual void operator()() {}

    static bool compare_sorting_order(
        const std::shared_ptr<DrawSceneSpace> a, 
        const std::shared_ptr<DrawSceneSpace> b) {
        return a->sorting_order < b->sorting_order;
    }

    int sorting_order = 0;
    std::string img_name;
    SDL_Texture* image;
    float x;
    float y;
};

//* DRAW *//
struct Fn_Draw : public DrawSceneSpace {
public:
    Fn_Draw(std::string _img_name, SDL_Texture* _image, 
        float _x, float _y) 
        : 
        DrawSceneSpace(_img_name, _image, _x, _y) {}

    void operator()();

}; //Fn_Draw

//* DRAW EX *//
struct Fn_DrawEx : public DrawSceneSpace {
public:
    Fn_DrawEx(std::string _img_name, SDL_Texture* _image, 
        float _x, float _y,  float _rotation, 
        float _scale_x, float _scale_y, 
        float _pivot_x, float _pivot_y, 
        float _r, float _g, float _b, 
        float _a, float _sorting_order) 
        :
        DrawSceneSpace(_img_name, _image, _x, _y),
        rotation((int)_rotation),
        scale_x(_scale_x),
        scale_y(_scale_y),
        pivot_x(_pivot_x),
        pivot_y(_pivot_y),
        r((int)_r),
        g((int)_g),
        b((int)_b),
        a((int)_a) 
    {
        sorting_order = (int)_sorting_order;
    }

    void operator()();

    int rotation;
    float scale_x;
    float scale_y;
    float pivot_x;
    float pivot_y;
    int r;
    int g;
    int b;
    int a;
}; //Fn_DrawEx

//* Base class for drawing UI
class DrawUISpace {
public:
    DrawUISpace(std::string _img_name,
       SDL_Texture* _image,
       float _x, float _y) 
       :
       img_name(_img_name),
       image(_image),
       x((int)_x),
       y((int)_y) {}

    virtual void operator()() {}

    static bool compare_sorting_order(
        const std::shared_ptr<DrawUISpace> a, 
        const std::shared_ptr<DrawUISpace> b) {
        return a->sorting_order < b->sorting_order;
    }

    std::string img_name;
    SDL_Texture* image;
    int x;
    int y;
    SDL_Color color = { 255,255,255,255 };
    int sorting_order = 0;
};

//* DRAW UI *//
struct Fn_DrawUI : DrawUISpace {
public:
    Fn_DrawUI(std::string _img_name,
        SDL_Texture* _image,
        float _x,
        float _y) :
        DrawUISpace(_img_name, _image, _x, _y) {}

    void operator()();
}; //Fn_DrawUI

//* DRAW UI EX *//
struct Fn_DrawUIEx : DrawUISpace {
public:
    Fn_DrawUIEx(std::string _img_name, SDL_Texture* _image, 
        float _x, float _y, float _r, float _g, 
        float _b, float _a, float _sorting_order) 
        :
        DrawUISpace(_img_name, _image, _x, _y),
        r((int)_r),
        g((int)_g),
        b((int)_b),
        a((int)_a) 
    {
        sorting_order = (int)_sorting_order;
    }

    void operator()();

    int r;
    int g;
    int b;
    int a;
}; //Fn_DrawUIEx


//* DRAW PIXEL *//
struct Fn_DrawPixel {
public:
    Fn_DrawPixel(float _x, float _y, 
        float _r, float _g, float _b, float _a) 
        :
        x((int)_x),
        y((int)_y),
        r((int)_r),
        g((int)_g),
        b((int)_b),
        a((int)_a) {}

    int x;
    int y;
    int r;
    int g;
    int b;
    int a;
}; //Fn_DrawPixel

struct Fn_DrawText {
public:
    Fn_DrawText(SDL_Texture* _text_to_render, float _x, float _y) 
        :
        text_to_render(_text_to_render),
        x((int)_x),
        y((int)_y) {}

    void operator()();

    SDL_Texture* text_to_render;
    int x;
    int y;
};

#endif /* Rednderer_h */
