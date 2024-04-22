#include "Tiled.hpp"

void Map::parse(std::string map) {
    
    std::string path = "Resources/maps/" + map + ".tmj";
    
    if (!std::filesystem::exists(path)) {
        std::cout << "error: failed to locate map" << std::endl;
        return;
    }
    
    // load map json
    rapidjson::Document map_doc;
    Utilities::read_json_file(path, map_doc);
    
    width = map_doc["width"].GetInt();
    height = map_doc["height"].GetInt();
    
    tile_width = map_doc["tilewidth"].GetInt();
    tile_height = map_doc["tileheight"].GetInt();
    
    // load tilesets
    const rapidjson::Value::Array map_tilesets = map_doc["tilesets"].GetArray();
    parse_tilesets(map_tilesets);
    
    const rapidjson::Value::Array map_layers = map_doc["layers"].GetArray();
    parse_layers(map_layers);
    
}

void Map::parse_layers(const rapidjson::Value::Array& map_layers) {
    for (auto& layer : map_layers) {
        std::string type = layer["type"].GetString();
        int id = layer["id"].GetInt();
        int x = layer["x"].GetInt();
        int y = layer["y"].GetInt();
        
        if (type == "tilelayer") {
            TileLayer new_layer(id, x, y, type);
            new_layer.width = layer["width"].GetInt();
            new_layer.height = layer["height"].GetInt();
            rapidjson::Value::Array data = layer["data"].GetArray();
            
            new_layer.set_data(data);
            layers.push_back(new_layer);
        } else if (type == "objectgroup") {
            ObjectLayer new_layer(id, x, y, type);
            layers.push_back(new_layer);
        }
    }
}

void Map::parse_tilesets(const rapidjson::Value::Array& map_tilesets) {
    for (auto& ts : map_tilesets) {
        // get image name
        std::string image_name = ts["image"].GetString();
        size_t name_idx = image_name.find_last_of("/") + 1;
        size_t png_idx = image_name.find(".png");
        size_t name_length = png_idx - name_idx;
        
        image_name = image_name.substr(name_idx, name_length);
        
        // get additional info
        int grid_id = ts["firstgid"].GetInt();
        int columns = ts["columns"].GetInt();
        int tilewidth = ts["tilewidth"].GetInt();
        int tileheight = ts["tileheight"].GetInt();
        int tilecount = ts["tilecount"].GetInt();
        int margin = ts["margin"].GetInt();
        int imagewidth = ts["imagewidth"].GetInt();
        int imageheight = ts["imageheight"].GetInt();
        
        
        // set properties (components)
        std::vector<std::string> props_to_add;
        if (ts.HasMember("properties")) {
            const rapidjson::Value::Array properties = ts["properties"].GetArray();
            for (auto& p : properties) {
                std::string prop_name = p["name"].GetString();
                props_to_add.emplace_back(prop_name);
            }
        }
        
        
        Tileset tileset(image_name, columns, grid_id, tilecount, tileheight, tilewidth, margin, imagewidth, imageheight);
        tileset.properties = props_to_add;
        tileset.load_tileset();
        tilesets.insert(std::make_pair(grid_id, tileset));
    }
}

Tileset Map::get_tileset(int gid) {
    int lower_gid = -1;
    for (auto& [id, tileset] : tilesets) {
        if (id <= gid && id > lower_gid) {
            lower_gid = id;
        }
    }
    return tilesets.find(lower_gid)->second;
}

SDL_Texture* Map::get_tile_texture(int gid) {
    return get_tileset(gid).get_tile_texture(gid);
}

void Map::populate_actors() {
    for (auto& layer : layers) {
        
        if (layer.type == "tilelayer") {
            
            std::vector<std::vector<int>> data = layer.get_data();
            
            for (int i = 0; i < data.size(); ++i) {
                for (int j = 0; j < data[0].size(); ++j) {
                    
                    int gid = data[i][j];
                    
                    if (gid != 0) {
                        int id = SceneDB::next_actor_id++;
                        SceneDB::actors[id] = std::make_shared<Actor>(Actor(id));
                        auto& actor_new = SceneDB::actors[id];
                        
                        //this division might break stuff
                        float tile_pos_x = (float)(j * tile_width);
                        float tile_pos_y = (float)(i * tile_height);
                        
                        glm::vec2 tilepos(tile_pos_x, tile_pos_y);
                        
                        Tileset tileset = get_tileset(gid);
                        actor_new->name = tileset.image_name;
                        
                        ComponentDB::load_tiled_properties(actor_new, tileset.properties, tilepos, gid);
                        
                        SceneDB::on_start_queue.push(actor_new);
                        SceneDB::actor_name_to_ids[actor_new->name].insert(actor_new->id);
                    }
                }
            }
        }
    }
}

std::vector<std::vector<int>> Layer::get_data() const {
    return data;
}

std::vector<std::vector<int>> TileLayer::get_data() const {
    return data;
}

void TileLayer::set_data(rapidjson::Value::Array &map_data) {
    data.resize(height);
    for (int i = 0; i < height; ++i) {
        data[i].resize(width);
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int idx = i * width + j;
            int val = map_data[idx].GetInt();
            data[i][j] = val;
        }
    }
}

void slice_tile(SDL_Texture* tileset, SDL_Texture* tile, SDL_Rect& tile_loc, int row, int col) {
    SDL_SetRenderTarget(Renderer::__Renderer, tile);
    SDL_SetTextureBlendMode(tile, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(
        Renderer::__Renderer,
        0,
        0,
        0, 
        0);
    SDL_RenderClear(Renderer::__Renderer);
    SDL_RenderCopy(Renderer::__Renderer, tileset, &tile_loc, NULL);
    SDL_SetRenderTarget(Renderer::__Renderer, NULL);
}

void Tileset::load_tileset() {
    ImageDB::load_image(image_name);
    tiles.resize(tilecount);
    
    SDL_Texture* tileset = ImageDB::images[image_name];
    
    int rows = tilecount / columns;
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            SDL_Rect tile_loc = {j * tilewidth, i * tileheight, tilewidth, tileheight};
            
            SDL_Texture* tile = SDL_CreateTexture(
                                Renderer::__Renderer,
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET,
                                tilewidth,
                                tileheight);
            
            slice_tile(tileset, tile, tile_loc, i, j);
            
            tiles[i + j] = tile;
            
            std::string name = image_name + std::to_string(grid_id + i + j);
            ImageDB::images[name] = tile;
        }
    }
}


SDL_Texture* Tileset::get_tile_texture(int gid) {
    
    SDL_Texture* tileset = ImageDB::images[image_name];
    int rel_id = gid - grid_id;
    
    // if the tile has already been cut from the set return it
    if (rel_id < tiles.size() && tiles[rel_id] != nullptr) {
        return tiles[rel_id];
    }
    
    // else create a new texture from the tileset
    int col = rel_id % columns;
    int row = rel_id / columns;
    
    SDL_Rect tile_loc = {col * tilewidth, row * tileheight, tilewidth, tileheight};
    
    SDL_Texture* tile = SDL_CreateTexture(
                        Renderer::__Renderer,
                        SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET,
                        tilewidth,
                        tileheight);
    slice_tile(tileset, tile, tile_loc, row, col);
    
    tiles[rel_id] = tile;
    
    std::string name = image_name + std::to_string(gid);
    ImageDB::images[name] = tile;
    
    return tile;
}


