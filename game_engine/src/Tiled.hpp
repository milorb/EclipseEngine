#ifndef Tiled_hpp
#define Tiled_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <unordered_map>

#include "EngineUtils.h"
#include "ImageDB.hpp"
#include "SceneDB.hpp"

struct Tileset;
struct Layer;
struct Object;

class Map {
public:
    void parse(std::string map);
    void parse_layers(const rapidjson::Value::Array& map_layers);
    void parse_tilesets(const rapidjson::Value::Array& map_tilesets);
    
    void populate_actors();
    SDL_Texture* get_tile_texture(int gid);
    Tileset get_tileset(int gid);
private:
    std::vector<Layer> layers;
    std::unordered_map<int, Tileset> tilesets;
    
    int width = 0;
    int height = 0;
    int tile_width = 0;
    int tile_height = 0;
};

struct Layer {
    Layer(int i, int x, int y, std::string t) : 
        id(i),
        pos(x, y),
        type(t) {}
    
    virtual std::vector<std::vector<int>> get_data() const;
    
    int id = 0;
    glm::vec2 pos = {0, 0};
    std::string type = "";
    
    std::vector<std::vector<int>> data;
};

struct TileLayer : Layer {
    using Layer::Layer;
    void set_data(rapidjson::Value::Array& data);
    std::vector<std::vector<int>> get_data() const override;
    
    int width = 0;
    int height = 0;
};

struct ObjectLayer : Layer {
    using Layer::Layer;
    std::vector<Object> objects;
};

struct Tileset {
    
    Tileset(std::string n, int c, int fgid, int tc, int th, int tw, int m, int iw, int ih) :
        image_name(n),
        columns(c),
        grid_id(fgid),
        tilecount(tc),
        tileheight(th),
        tilewidth(tw),
        margin(m),
        imagewidth(iw),
        imageheight(ih) {}
    
    void load_tileset();
    
    std::vector<std::string> get_properties(int gid);
    
    SDL_Texture* get_tile_texture(int gid);
    
    std::string image_name;
    int columns;
    int grid_id = 0;
    int tilecount;
    int tileheight;
    int tilewidth;
    int margin;
    int imagewidth;
    int imageheight;
    
    std::vector<SDL_Texture*> tiles;
    std::vector<std::string> properties;
};

struct Object {
    
    
    
    int gid;
};



#endif /* Tiled_hpp */
