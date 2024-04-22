#include "ImageDB.hpp"

void ImageDB::init() {

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Image")
        .addFunction("DrawUI", 
            &ImageDB::DrawUI)
        .addFunction("DrawUIEx", 
            &ImageDB::DrawUIEx)
        .addFunction("Draw", 
            &ImageDB::Draw)
        .addFunction("DrawEx", 
            &ImageDB::DrawEx)
        .addFunction("DrawPixel", 
            &ImageDB::DrawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(ComponentDB::__State)
        .beginNamespace("Camera")
        .addFunction("SetPosition", 
            &Renderer::CamSetPosition)
        .addFunction("GetPositionX", 
            &Renderer::CamGetPositionX)
        .addFunction("GetPositionY", 
            &Renderer::CamGetPositionY)
        .addFunction("SetZoom", 
            &Renderer::CamSetZoom)
        .addFunction("GetZoom", 
            &Renderer::CamGetZoom)
        .endNamespace();
}

void ImageDB::load_image(std::string filename) {

    std::string filepath = "resources/images/" + filename + ".png";
    if (std::filesystem::exists(filepath)) {
        SDL_Texture *texture = IMG_LoadTexture(Renderer::__Renderer, filepath.c_str());
        if (!texture) {
            std::cout << "error: failed to initialize texture " << SDL_GetError();
            std::exit(0);
        }
        images[filename] = texture;
    }
    else {
        std::cout << "no such file";
        std::exit(0);
    }
}

void ImageDB::DrawUI(std::string img_name, float x, float y) {

    if (images.find(img_name) == images.end()) {
        load_image(img_name);
    }
    SDL_Texture* img = images[img_name];

    std::shared_ptr <Fn_DrawUI> fn = std::make_shared <Fn_DrawUI>(img_name, img, x, y);

    Renderer::ui_q.emplace_back(fn);

}

void ImageDB::DrawUIEx(std::string img_name, float x, float y,
    float r, float g, float b, float a, float sorting_order) {

    if (images.find(img_name) == images.end()) {
        load_image(img_name);
    }
    SDL_Texture* img = images[img_name];

    std::shared_ptr <Fn_DrawUIEx> fn = std::make_shared <Fn_DrawUIEx>(img_name, img, x, y, r, g, b, a, sorting_order);

    Renderer::ui_q.emplace_back(fn);
}

void ImageDB::Draw(std::string img_name, float x, float y) {

    if (images.find(img_name) == images.end()) {
        load_image(img_name);
    }
    SDL_Texture* img = images[img_name];

    std::shared_ptr <Fn_Draw> fn = std::make_shared <Fn_Draw>(img_name, img, x, y);

    Renderer::scene_q.emplace_back(fn);
}

void ImageDB::DrawEx(std::string img_name, float x, float y, float rotation,
    float scale_x, float scale_y, float pivot_x, float pivot_y,
    float r, float g, float b, float a, float sorting_order) {

    if (images.find(img_name) == images.end()) {
        load_image(img_name);
    }
    SDL_Texture* img = images[img_name];

    std::shared_ptr<Fn_DrawEx> fn = std::make_shared<Fn_DrawEx>(
        img_name, img, 
        x, y, rotation, scale_x, scale_y, 
        pivot_x, pivot_y, r, g, b, a, 
        sorting_order);

    Renderer::scene_q.emplace_back(fn);
}

void ImageDB::DrawPixel(float x, float y, float r, float g, float b, float a) {

    Fn_DrawPixel* fn = new Fn_DrawPixel(x, y, r, g, b, a);

    Renderer::px_q.emplace_back(fn);
}
